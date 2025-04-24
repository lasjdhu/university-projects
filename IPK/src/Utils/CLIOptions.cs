/**
 * Command Line Interface options
 * Defines available command line arguments and their properties
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using CommandLine;

namespace IPK25ChatClient.CLI;

/**
 * CLI Options for the chat client
 */
public class CLIOptions {
  [Option('t', "transport", Required = true, HelpText = "Transport protocol used for connection: tcp, udp")]
  public required string Transport {
    get;
    set;
  }

  [Option('s', "server", Required = true, HelpText = "Server IP or hostname")]
  public required string Server {
    get;
    set;
  }

  [Option('p', "port", Default = 4567, HelpText = "Server port")]
  public int Port {
    get;
    set;
  }

  [Option('d', "delay", Default = 250, HelpText = "UDP confirmation timeout (in milliseconds)")]
  public int Delay {
    get;
    set;
  }

  [Option('r', "retries", Default = 3, HelpText = "Maximum number of UDP retransmissions")]
  public int Retries {
    get;
    set;
  }

  [Option('h', "help", HelpText = "Prints program help output and exits")]
  public bool DisplayHelp {
    get;
    set;
  }
}
