/**
 * TCP implementation of the chat client
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.Extensions.Logging;
using IPK25ChatClient.Protocol;

namespace IPK25ChatClient.Network;

/**
 * TCP-based chat client implementation
 */
public class TCPClient: ClientBase {
  private TcpClient ? _client;
  private NetworkStream ? _stream;
  private StreamReader ? _reader;
  private StreamWriter ? _writer;
  private readonly CancellationTokenSource _cts = new();

  public TCPClient(string server, int port, ILogger < IClient > logger): base(server, port, logger) {}

  /**
   * Initializes TCP connection and starts message receiving loop
   */
  public override async Task Run() {
    try {
      var ipEndPoint = await GetEndPointAsync();

      // initialize tcp client and connect to server
      _client = new TcpClient();
      await _client.ConnectAsync(ipEndPoint);

      // setup stream reader/writer with ascii encoding
      _stream = _client.GetStream();
      _reader = new StreamReader(_stream, Encoding.ASCII);
      _writer = new StreamWriter(_stream, Encoding.ASCII) {
        AutoFlush = true
      };

      IsConnected = true;
      _logger.LogInformation("[TCP] Connected to {Server}:{Port}", _server, _port);

      // start receiving messages in background
      _ = Task.Run(ReceiveLoop);
    } catch (Exception e) {
      _logger.LogError("[TCP] Connection error: {Message}", e.Message);
      await CloseConnection();
    }
  }

  /**
   * Background loop for receiving messages
   */
  private async Task ReceiveLoop() {
    if (_reader == null) return;

    try {
      while (IsConnected && !_cts.Token.IsCancellationRequested) {
        string ? message = await _reader.ReadLineAsync(_cts.Token);
        if (message == null) break;

        await ProcessReceivedMessage(message);
      }
    } catch (OperationCanceledException) {} catch (Exception ex) {
      if (IsConnected) {
        _logger.LogError("[TCP] Error in receive loop: {Message}", ex.Message);
        await CloseConnection();
      }
    }
  }

  /**
   * Processes received message based on its type
   *
   * @param message
   */
  private async Task ProcessReceivedMessage(string message) {
    try {
      var (type, sender, content) = TCPMessageHandler.ParseMessage(message);

      switch (type) {
      case "REPLY":
        if (sender == "OK") {
          _logger.LogInformation("Action Success: {Message}", content);
        } else if (sender == "NOK") {
          _logger.LogInformation("Action Failure: {Message}", content);
        } else {
          _logger.LogError("ERROR: Malformed REPLY message");
          await CloseConnection();
        }
        break;

      case "MSG":
        _logger.LogInformation("{Sender}: {Content}", sender, content);
        break;

      case "ERR":
        _logger.LogError("ERROR FROM {Sender}: {Message}", sender, content);
        await CloseConnection();
        break;

      case "BYE":
        _logger.LogInformation("[TCP] Received BYE from {Sender}", sender);
        await CloseConnection();
        break;
      }
    } catch (Exception ex) {
      _logger.LogError("ERROR: {Message}", ex.Message);
      await SendErrorMessage($"Failed to process message: {ex.Message}");
      await CloseConnection();
    }
  }

  /**
   * Sends authentication message to server
   */
  protected override async Task SendAuthMessage(string username, string secret, string displayName) {
    string message = TCPMessageHandler.FormatAuthMessage(username, displayName, secret);
    await SendMessage(message);
  }

  /**
   * Sends channel join message to server
   */
  protected override async Task SendJoinMessage(string channelId) {
    string message = TCPMessageHandler.FormatJoinMessage(channelId, DisplayName);
    await SendMessage(message);
  }

  /**
   * Sends chat message to server
   */
  protected override async Task SendChatMessage(string message) {
    string formattedMessage = TCPMessageHandler.FormatChatMessage(DisplayName, message);
    await SendMessage(formattedMessage);
  }

  /**
   * Closes TCP connection and cleans up resources, sends BYE message if authenticated
   */
  public override async Task CloseConnection() {
    if (!IsConnected) return;

    try {
      if (_writer != null && !string.IsNullOrEmpty(DisplayName)) {
        string message = TCPMessageHandler.FormatByeMessage(DisplayName);
        await SendMessage(message);
      }
    } catch (Exception ex) {
      _logger.LogError("[TCP] Error during connection close: {Message}", ex.Message);
    } finally {
      IsConnected = false;
      _cts.Cancel();

      _writer?.Dispose();
      _reader?.Dispose();
      _stream?.Dispose();
      _client?.Dispose();

      _writer = null;
      _reader = null;
      _stream = null;
      _client = null;

      _logger.LogInformation("[TCP] Connection closed.");
    }
  }

  /**
   * Sends formatted message to server
   *
   * @param message
   */
  private async Task SendMessage(string message) {
    if (_writer == null || !IsConnected) {
      _logger.LogError("ERROR: Client not initialized");
      return;
    }

    try {
      await _writer.WriteAsync(message);
      await _writer.FlushAsync();
      _logger.LogInformation("[TCP] Sent message: {Message}", message);
    } catch (Exception ex) {
      _logger.LogError("ERROR: Failed to send message: {Message}", ex.Message);
      await CloseConnection();
    }
  }

  /**
   * Sends error message to server
   *
   * @param errorMessage
   */
  private async Task SendErrorMessage(string errorMessage) {
    if (_writer == null || !IsConnected) return;

    try {
      string message = TCPMessageHandler.FormatErrorMessage(
        string.IsNullOrEmpty(DisplayName) ? "Client" : DisplayName,
        errorMessage);
      await _writer.WriteAsync(message);
      await _writer.FlushAsync();
    } catch (Exception ex) {
      _logger.LogError("ERROR: Failed to send error message: {Message}", ex.Message);
    }
  }
}
