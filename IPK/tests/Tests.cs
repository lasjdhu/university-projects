using IPK25ChatClient.Network;
using IPK25ChatClient.CLI;
using Microsoft.Extensions.Logging;
using Xunit;
using Xunit.Abstractions;
using System;

namespace IPK25ChatClient.Tests;

public class ChatClientTests {
  private readonly ITestOutputHelper _output;
  private readonly ILogger < IClient > _logger;
  private readonly CLIHandler _cliHandler;

  public ChatClientTests(ITestOutputHelper output) {
    _output = output;
    _logger = LoggerFactory.Create(builder =>
        builder.AddConsole()
        .SetMinimumLevel(LogLevel.Debug))
      .CreateLogger < IClient > ();
    _cliHandler = new CLIHandler();
  }

  [Fact]
  public void Help() {
    Console.WriteLine("\n=== Testing /help command - should display all available commands ===");
    var client = new TCPClient("localhost", 12345, _logger);

    client.Help();
    // When we call Help(), it should print all available commands to console
  }

  [Fact]
  public void TCPClientCreation() {
    Console.WriteLine("\n=== Testing TCP client creation with host=localhost, port=12345 ===");
    var client = new TCPClient("localhost", 12345, _logger);

    Assert.NotNull(client);
    // Client is created but not connected yet
  }

  [Fact]
  public void UDPClientCreation() {
    Console.WriteLine("\n=== Testing UDP client creation with host=localhost, port=12345, delay=100ms, retries=3 ===");
    var client = new UDPClient("localhost", 12345, 100, 3, _logger);

    Assert.NotNull(client);
    // Client is created but not connected yet
  }

  [Theory]
  [InlineData("test message")]
  [InlineData("Hello, World!")]
  public async Task SendWithoutAuth(string input) {
    Console.WriteLine($"\n=== Testing message send without auth - should log authentication error ===");
    Console.WriteLine($"Message: '{input}'");

    var client = new TCPClient("localhost", 12345, _logger);
    await client.Send(input);
    // "ERROR: You are not authenticated" is the expected behavior when trying to send without authentication
  }
}
