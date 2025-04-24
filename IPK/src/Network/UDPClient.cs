/**
 * UDP implementation of the chat client
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using System.Text;
using System.Net;
using System.Net.Sockets;
using Microsoft.Extensions.Logging;
using System.Collections.Concurrent;
using IPK25ChatClient.Protocol;

namespace IPK25ChatClient.Network;

/**
 * UDP-based chat client implementation
 */
public class UDPClient: ClientBase {
  private readonly int _confirmationTimeout;  // timeout in ms for message confirmation
  private readonly int _retryCount;           // max number of retries for unconfirmed messages
  private UdpClient ? _client;
  private IPEndPoint ? _remoteEndPoint;       // initial server endpoint
  private IPEndPoint ? _dynamicEndPoint;      // server's dynamic port for responses
  private ushort _nextMessageId = 0;
  private ConcurrentDictionary < ushort, TaskCompletionSource < bool >> _pendingConfirmations = new();
  private ConcurrentDictionary < ushort, bool > _processedMessageIds = new();

  public UDPClient(string server, int port, int confirmationTimeout, int retryCount, ILogger < IClient > logger): base(server, port, logger) {
    _confirmationTimeout = confirmationTimeout;
    _retryCount = retryCount;
  }

  /**
   * Initializes UDP socket and starts message receiving loop
   */
  public override async Task Run() {
    try {
      _remoteEndPoint = await GetEndPointAsync();
      _client = new UdpClient();
      _client.Client.Bind(new IPEndPoint(IPAddress.Any, 0));
      IsConnected = true;
      _logger.LogInformation("[UDP] Connected to {Server}:{Port}", _server, _port);
      _ = Task.Run(ReceiveLoop);
    } catch (Exception ex) {
      _logger.LogError("[UDP] Connection error: {Message}", ex.Message);
      await CloseConnection();
    }
  }

  /**
   * Background loop for receiving messages
   */
  private async Task ReceiveLoop() {
    while (IsConnected) {
      try {
        if (_client == null) return;
        var result = await _client.ReceiveAsync();
        await ProcessReceivedMessage(result.Buffer, result.RemoteEndPoint);
      } catch (Exception ex) {
        _logger.LogError("[UDP] Error in receive loop: {Message}", ex.Message);
        if (_client?.Client?.Connected == false) break;
      }
    }
  }

  /**
   * Processes received UDP Message
   *
   * @param data
   * @param remoteEndPoint
   */
  private async Task ProcessReceivedMessage(byte[] data, IPEndPoint remoteEndPoint) {
    byte messageType = data[0];
    ushort messageId = BitConverter.ToUInt16(new [] {
      data[2], data[1]
    }, 0);

    if (_dynamicEndPoint == null && messageType == UDPMessageHandler.MSG_TYPE_REPLY) {
      _dynamicEndPoint = remoteEndPoint;
      _logger.LogInformation("[UDP] Server assigned dynamic port: {Port}", remoteEndPoint.Port);
    }

    // handle duplicate messages
    if (_processedMessageIds.ContainsKey(messageId)) {
      if (IsConnected && messageType != UDPMessageHandler.MSG_TYPE_CONFIRM) {
        await SendConfirmation(messageId);
      }
      _logger.LogInformation("[UDP] Skipping duplicate message {MessageId}", messageId);
      return;
    }

    _processedMessageIds[messageId] = true;

    if (messageType != UDPMessageHandler.MSG_TYPE_CONFIRM && IsConnected) {
      await SendConfirmation(messageId);
    }

    switch (messageType) {
    case UDPMessageHandler.MSG_TYPE_CONFIRM:
      if (_pendingConfirmations.TryRemove(messageId, out
          var tcs)) {
        tcs.TrySetResult(true);
      }
      _logger.LogInformation("[UDP] Received confirmation for message {MessageId}", messageId);
      break;

    case UDPMessageHandler.MSG_TYPE_REPLY:
      byte result = data[3];
      ushort refMessageId = BitConverter.ToUInt16(new [] {
        data[5], data[4]
      }, 0);
      string message = UDPMessageHandler.ExtractNullTerminatedString(data, 6);
      _logger.LogInformation("[UDP] Received REPLY (ref: {RefMessageId}) - {Status}: {Message}",
        refMessageId,
        result == 1 ? "Success" : "Failure",
        message);
      break;

    default:
      _logger.LogError("[UDP] Received unsupported message type: {Type}", messageType);
      break;
    }
  }

  /**
   * Sends authentication message to server
   */
  protected override async Task SendAuthMessage(string username, string secret, string displayName) {
    byte[] message = UDPMessageHandler.FormatAuthMessage(_nextMessageId, username, displayName, secret);
    _logger.LogInformation("[UDP] Sending auth message {MessageId}", _nextMessageId);
    await SendMessage(message);
  }

  /**
   * Join command not implemented in UDP protocol
   */
  protected override Task SendJoinMessage(string channelId) {
    _logger.LogWarning("[UDP] JOIN command is not implemented");
    return Task.CompletedTask;
  }

  /**
   * Chat message not implemented in UDP protocol
   */
  protected override Task SendChatMessage(string message) {
    _logger.LogWarning("[UDP] MSG command is not implemented");
    return Task.CompletedTask;
  }

  /**
   * Closes UDP connection and cleans up resources
   */
  public override Task CloseConnection() {
    if (!IsConnected) return Task.CompletedTask;

    IsConnected = false;
    _client?.Close();
    _client = null;
    _logger.LogInformation("[UDP] Connection closed.");
    return Task.CompletedTask;
  }

  /**
   * Sends message
   *
   * @param message
   */
  private async Task SendMessage(byte[] message) {
    if (_client == null || (_remoteEndPoint == null && _dynamicEndPoint == null)) {
      _logger.LogError("ERROR: Client not initialized");
      return;
    }

    var endpoint = _dynamicEndPoint ?? _remoteEndPoint!;
    bool confirmed = false;
    var currentMessageId = _nextMessageId;

    var confirmationSource = new TaskCompletionSource < bool > ();
    _pendingConfirmations[currentMessageId] = confirmationSource;

    // retry loop
    for (int attempt = 0; attempt <= _retryCount; attempt++) {
      try {
        await _client.SendAsync(message, message.Length, endpoint);
        _logger.LogInformation("[UDP] Sent message {MessageId}, attempt {Attempt}", currentMessageId, attempt + 1);

        try {
          // wait for confirmation with timeout
          await Task.Delay(_confirmationTimeout);
          if (confirmationSource.Task.IsCompleted) {
            confirmed = true;
            break;
          }
          _logger.LogWarning("[UDP] Message {MessageId} confirmation timeout, attempt {Attempt}/{Retries}",
            currentMessageId, attempt + 1, _retryCount);
        } catch (Exception) {
          _logger.LogError("[UDP] Message {MessageId} confirmation timeout, attempt {Attempt}/{Retries}",
            currentMessageId, attempt + 1, _retryCount);
        }
      } catch (Exception ex) {
        _logger.LogError("[UDP] Failed to send message: {Message}", ex.Message);
      }
    }

    _pendingConfirmations.TryRemove(currentMessageId, out _);

    // handle message failure after all retries
    if (!confirmed) {
      _logger.LogError("[UDP] Message {MessageId} failed after {Retries} retries", currentMessageId, _retryCount);
      await CloseConnection();
      return;
    }

    _nextMessageId++;
  }

  /**
   * Sends confirmation for received message
   *
   * @param messageIdToConfirm
   */
  private async Task SendConfirmation(ushort messageIdToConfirm) {
    if (_client == null) return;
    byte[] confirmData = UDPMessageHandler.FormatConfirmation(messageIdToConfirm, messageIdToConfirm);
    _logger.LogInformation("[UDP] Sending confirmation for message {MessageId}", messageIdToConfirm);
    await _client.SendAsync(confirmData, confirmData.Length, _dynamicEndPoint ?? _remoteEndPoint!);
  }
}
