/**
 * Base networking functionality for chat clients
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using System.Net;
using System.Net.Sockets;
using Microsoft.Extensions.Logging;

namespace IPK25ChatClient.Network;

/**
 * Interface defining chat client operations
 */
public interface IClient {
  Task Run();
  Task Auth(string username, string secret, string displayName);
  Task Join(string channelId);
  Task Rename(string displayName);
  Task Send(string message);
  Task CloseConnection();
  void Help();
}

/**
 * Abstract base class implementing common chat client functionality
 */
public abstract class ClientBase: IClient {
  protected readonly string _server;
  protected readonly int _port;
  protected readonly ILogger < IClient > _logger;

  protected string Username {
    get;
    set;
  } = "";
  protected string DisplayName {
    get;
    set;
  } = "";
  protected string Secret {
    get;
    set;
  } = "";
  protected string ChannelID {
    get;
    set;
  } = "";
  protected bool IsConnected {
    get;
    set;
  }

  protected ClientBase(string server, int port, ILogger < IClient > logger) {
    _server = server;
    _port = port;
    _logger = logger;
  }

  /**
   * Resolves server hostname to IPv4 endpoint
   */
  protected async Task < IPEndPoint > GetEndPointAsync() {
    IPAddress[] addresses = await Dns.GetHostAddressesAsync(_server);
    // find first IPv4 address in the list
    IPAddress address = addresses.FirstOrDefault(a => a.AddressFamily == AddressFamily.InterNetwork) ??
      throw new InvalidOperationException($"No IPv4 address found for {_server}");

    return new IPEndPoint(address, _port);
  }

  /**
   * Initializes and runs the client
   */
  public abstract Task Run();

  /**
   * Authenticates user with the server
   *
   * @param username
   * @param secret
   * @param displayName
   */
  public virtual async Task Auth(string username, string secret, string displayName) {
    Username = username;
    Secret = secret;
    DisplayName = displayName;
    await SendAuthMessage(username, secret, displayName);
  }

  /**
   * Joins specified chat channel
   *
   * @param channelId
   */
  public virtual async Task Join(string channelId) {
    ChannelID = channelId;
    await SendJoinMessage(channelId);
  }

  /**
   * Changes user's display name
   *
   * @param displayName
   */
  public virtual Task Rename(string displayName) {
    DisplayName = displayName;
    _logger.LogInformation("Display name changed to: {DisplayName}", displayName);
    return Task.CompletedTask;
  }

  /**
   * Sends chat message
   *
   * @param message Message to send
   */
  public virtual async Task Send(string message) {
    if (string.IsNullOrEmpty(DisplayName)) {
      _logger.LogError("ERROR: You are not authenticated");
      return;
    }

    await SendChatMessage(message);
  }

  /**
   * Displays help information about available commands
   */
  public virtual void Help() {
    Console.WriteLine(
      "Available commands:\n" +
      "/auth <username> <secret> <displayname> - Authenticate with the server\n" +
      "/join <channelid> - Join a channel\n" +
      "/rename <displayname> - Change your display name\n" +
      "/help - Show this help message\n" +
      "/exit - Forcefully close the connection\n" +
      "To send a message, you can type it directly here\n" +
      "You can use <C-c> and <C-d> to close the connection"
    );
  }

  public abstract Task CloseConnection();
  protected abstract Task SendAuthMessage(string username, string secret, string displayName);
  protected abstract Task SendJoinMessage(string channelId);
  protected abstract Task SendChatMessage(string message);
}
