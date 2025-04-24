/**
 * Main program class for IPK25 Chat Client
 * Handles client initialization, command processing, and connection management
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using IPK25ChatClient.CLI;
using IPK25ChatClient.Network;
using System;
using System.Threading.Tasks;

namespace IPK25ChatClient;
/**
 * Entry point of the application
 */
public static class Program {
  /**
   * Main entry point
   *
   * @param args
   */
  public static void Main(string[] args) {
    var builder = Host.CreateApplicationBuilder();
    builder.Services.AddLogging(loggingBuilder => loggingBuilder
      .ClearProviders()
      .AddSimpleConsole(opts => opts.SingleLine = true)
      .AddConsole(opts => opts.LogToStandardErrorThreshold = LogLevel.Trace));
    var host = builder.Build();
    var logger = host.Services.GetRequiredService < ILogger < IClient >> ();
    var cliHandler = new CLIHandler();
    cliHandler.Handle(args, options => Run(options, cliHandler, logger).Wait());
  }

  /**
   * Main client execution loop
   *
   * @param options
   * @param cliHandler
   * @param logger
   */
  private static async Task Run(CLIOptions options, CLIHandler cliHandler, ILogger < IClient > logger) {
    IClient ? client = options.Transport
    switch {
      "tcp" => new TCPClient(options.Server, options.Port, logger),
      "udp" => new UDPClient(options.Server, options.Port, options.Delay, options.Retries, logger),
      _ => null,
    };

    if (client == null) {
      logger.LogError("Invalid transport type. See help");
      cliHandler.DisplayHelp();
      Environment.Exit(1);
    }

    Console.CancelKeyPress += async (sender, e) => {
      e.Cancel = true;
      logger.LogInformation("Shutting down by key press...");
      await HandleShutdown(client, logger);
      Environment.Exit(0);
    };

    try {
      await client.Run();
    } catch (Exception ex) {
      logger.LogError("Failed to initialize client: {Message}", ex.Message);
      Environment.Exit(1);
    }

    await HandleInput(client, logger);
    await HandleShutdown(client, logger);
  }

  /**
   * Handles user input
   *
   * @param client
   * @param logger
   */
  private static async Task HandleInput(IClient client, ILogger < IClient > logger) {
    try {
      while (true) {
        string ? input = Console.ReadLine();
        if (string.IsNullOrEmpty(input)) continue;

        if (input.StartsWith("/")) {
          if (input == "/exit") {
            break;
          }
          await Execute(input, client, logger);
        } else {
          await client.Send(input);
        }
      }
    } catch (Exception ex) {
      logger.LogError(ex.Message);
    }
  }

  /**
   * Executes chat commands
   *
   * @param input
   * @param client
   * @param logger
   */
  private static async Task Execute(string input, IClient client, ILogger < IClient > logger) {
    var arguments = input.Split(' ');
    switch (arguments[0].ToLower()) {
    case "/auth":
      if (!CheckArgsNumber(arguments, 4, logger)) return;
      string username = arguments[1];
      string secret = arguments[2];
      string displayName = arguments[3];
      await client.Auth(username, secret, displayName);
      break;

    case "/join":
      if (!CheckArgsNumber(arguments, 2, logger)) return;
      string channelID = arguments[1];
      await client.Join(channelID);
      break;

    case "/rename":
      if (!CheckArgsNumber(arguments, 2, logger)) return;
      string newDisplayName = arguments[1];
      await client.Rename(newDisplayName);
      break;

    case "/help":
      client.Help();
      break;

    default:
      logger.LogError("Unknown command: {Command}", arguments[0]);
      client.Help();
      break;
    }
  }

  /**
   * Handles graceful shutdown of the client
   *
   * @param client
   * @param logger
   */
  private static async Task HandleShutdown(IClient client, ILogger < IClient > logger) {
    try {
      await client.CloseConnection();
    } catch (Exception e) {
      logger.LogError("Error during shutdown: {Message}", e.Message);
      Environment.Exit(1);
    }
  }

  /**
   * Validates command argument count
   *
   * @param arguments
   * @param expectedNumber
   * @param logger
   */
  private static bool CheckArgsNumber(string[] arguments, int expectedNumber, ILogger < IClient > logger) {
    if (arguments.Length != expectedNumber) {
      logger.LogError("Invalid number of arguments for {Command}. Expected {Expected}, got {Actual}",
        arguments[0], expectedNumber, arguments.Length);
      return false;
    }
    return true;
  }
}
