/**
 * Handles UDP message formatting and binary protocol implementation
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using System.Text;

namespace IPK25ChatClient.Protocol;

/**
 * UDP message handler class
 */
public static class UDPMessageHandler {
  // headers
  public
  const byte MSG_TYPE_CONFIRM = 0x00;
  public
  const byte MSG_TYPE_REPLY = 0x01;
  public
  const byte MSG_TYPE_AUTH = 0x02;
  public
  const byte MSG_TYPE_JOIN = 0x03;
  public
  const byte MSG_TYPE_MSG = 0x04;
  public
  const byte MSG_TYPE_PING = 0xFD;
  public
  const byte MSG_TYPE_ERR = 0xFE;
  public
  const byte MSG_TYPE_BYE = 0xFF;

  /**
   * Creates binary authentication message
   *
   * @param messageId
   * @param username
   * @param displayName
   * @param secret
   */
  public static byte[] FormatAuthMessage(ushort messageId, string username, string displayName, string secret) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_AUTH);
    WriteMessageId(ms, messageId);
    WriteNullTerminated(ms, username);
    WriteNullTerminated(ms, displayName);
    WriteNullTerminated(ms, secret);
    return ms.ToArray();
  }

  /**
   * Creates binary join message
   *
   * @param messageId
   * @param channelId
   * @param displayName
   */
  public static byte[] FormatJoinMessage(ushort messageId, string channelId, string displayName) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_JOIN);
    WriteMessageId(ms, messageId);
    WriteNullTerminated(ms, channelId);
    WriteNullTerminated(ms, displayName);
    return ms.ToArray();
  }

  /**
   * Creates binary chat message
   *
   * @param messageId
   * @param displayName
   * @param message
   */
  public static byte[] FormatChatMessage(ushort messageId, string displayName, string message) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_MSG);
    WriteMessageId(ms, messageId);
    WriteNullTerminated(ms, displayName);
    WriteNullTerminated(ms, message);
    return ms.ToArray();
  }

  /**
   * Creates binary BYE message
   *
   * @param messageId
   * @param displayName
   */
  public static byte[] FormatByeMessage(ushort messageId, string displayName) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_BYE);
    WriteMessageId(ms, messageId);
    WriteNullTerminated(ms, displayName);
    return ms.ToArray();
  }

  /**
   * Creates binary error message
   *
   * @param messageId
   * @param displayName
   * @param errorMessage
   */
  public static byte[] FormatErrorMessage(ushort messageId, string displayName, string errorMessage) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_ERR);
    WriteMessageId(ms, messageId);
    WriteNullTerminated(ms, displayName);
    WriteNullTerminated(ms, errorMessage);
    return ms.ToArray();
  }

  /**
   * Creates binary confirmation message
   *
   * @param messageId
   * @param messageIdToConfirm
   */
  public static byte[] FormatConfirmation(ushort messageId, ushort messageIdToConfirm) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_CONFIRM);
    WriteMessageId(ms, messageIdToConfirm);
    return ms.ToArray();
  }

  /**
   * Creates binary reply message
   *
   * @param messageId
   * @param result
   * @param refMessageId
   * @param messageContents
   */
  public static byte[] FormatReplyMessage(ushort messageId, byte result, ushort refMessageId, string messageContents) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_REPLY);
    WriteMessageId(ms, messageId);
    ms.WriteByte(result);
    WriteMessageId(ms, refMessageId);
    WriteNullTerminated(ms, messageContents);
    return ms.ToArray();
  }

  /**
   * Creates binary ping message
   *
   * @param messageId
   */
  public static byte[] FormatPingMessage(ushort messageId) {
    using
    var ms = new MemoryStream();
    ms.WriteByte(MSG_TYPE_PING);
    WriteMessageId(ms, messageId);
    return ms.ToArray();
  }

  /**
   * Extracts null-terminated string from binary data
   *
   * @param data
   * @param startIndex
   */
  public static string ExtractNullTerminatedString(byte[] data, int startIndex) {
    int endIndex = startIndex;
    while (endIndex < data.Length && data[endIndex] != 0) {
      endIndex++;
    }
    return Encoding.ASCII.GetString(data, startIndex, endIndex - startIndex);
  }

  /**
   * Writes message ID in big-endian format
   *
   * @param ms
   * @param messageId
   */
  private static void WriteMessageId(MemoryStream ms, ushort messageId) {
    byte[] msgIdBytes = BitConverter.GetBytes(messageId);
    ms.WriteByte(msgIdBytes[1]);
    ms.WriteByte(msgIdBytes[0]);
  }

  /**
   * Writes null-terminated string to stream
   *
   * @param ms
   * @param value
   */
  private static void WriteNullTerminated(MemoryStream ms, string value) {
    byte[] bytes = Encoding.ASCII.GetBytes(value);
    ms.Write(bytes, 0, bytes.Length);
    ms.WriteByte(0);
  }
}
