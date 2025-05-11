/**
 * Handles TCP message formatting and parsing
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

namespace IPK25ChatClient.Protocol;

/**
 * TCP message handler class
 */
public static class TCPMessageHandler {
  /**
   * Formats authentication message according to protocol
   *
   * @param username
   * @param displayName
   * @param secret
   */
  public static string FormatAuthMessage(string username, string displayName, string secret) {
    return $"AUTH {username} AS {displayName} USING {secret}\r\n";
  }

  /**
   * Formats channel join message
   *
   * @param channelId
   * @param displayName
   */
  public static string FormatJoinMessage(string channelId, string displayName) {
    return $"JOIN {channelId} AS {displayName}\r\n";
  }

  /**
   * Formats chat message
   *
   * @param displayName
   * @param message
   */
  public static string FormatChatMessage(string displayName, string message) {
    return $"MSG FROM {displayName} IS {message}\r\n";
  }

  /**
   * Formats BYE message
   *
   * @param displayName
   */
  public static string FormatByeMessage(string displayName) {
    return $"BYE FROM {displayName}\r\n";
  }

  /**
   * Formats error message
   *
   * @param displayName
   * @param errorMessage
   */
  public static string FormatErrorMessage(string displayName, string errorMessage) {
    return $"ERR FROM {displayName} IS {errorMessage}\r\n";
  }

  /**
   * Parses received message into components
   *
   * @param message
   */
  public static(string type, string sender, string content) ParseMessage(string message) {
    if (message.StartsWith("REPLY ")) {
      int isIndex = message.IndexOf(" IS ");
      if (isIndex == -1)
        throw new FormatException("Malformed REPLY message");

      string status = message.Substring(6, isIndex - 6).Trim();
      string content = message.Substring(isIndex + 4);

      return ("REPLY", status, content);
    } else if (message.StartsWith("MSG FROM ")) {
      int isIndex = message.IndexOf(" IS ");
      if (isIndex == -1)
        throw new FormatException("Malformed MSG message");

      string sender = message.Substring(9, isIndex - 9).Trim();
      string content = message.Substring(isIndex + 4);

      return ("MSG", sender, content);
    } else if (message.StartsWith("ERR FROM ")) {
      int isIndex = message.IndexOf(" IS ");
      if (isIndex == -1)
        throw new FormatException("Malformed ERR message");

      string sender = message.Substring(9, isIndex - 9).Trim();
      string content = message.Substring(isIndex + 4);

      return ("ERR", sender, content);
    } else if (message.StartsWith("BYE FROM ")) {
      string sender = message.Substring(9).Trim();
      return ("BYE", sender, string.Empty);
    }

    throw new FormatException($"Unknown message type: {message}");
  }
}
