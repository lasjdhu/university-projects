/**
 * Command Line Interface handler
 *
 * @author: Dmitrii Ivanushkin (xivanu00)
 */

using CommandLine;
using CommandLine.Text;

namespace IPK25ChatClient.CLI;

/**
 * Handles command line argument parsing and help display
 */
public class CLIHandler {
  private ParserResult < CLIOptions > ? _parserResult;

  /**
   * Handles command line arguments and executes provided action
   *
   * @param args
   * @param Run
   */
  public void Handle(string[] args, Action < CLIOptions > Run) {
    var parser = new Parser(with => {
      with.HelpWriter = null;
      with.AutoHelp = false;
      with.AutoVersion = false;
    });

    _parserResult = parser.ParseArguments < CLIOptions > (args);

    _parserResult
      .WithParsed(Run)
      .WithNotParsed(errs => DisplayHelp());
  }

  /**
   * Displays formatted help information
   */
  public void DisplayHelp() {
    if (_parserResult == null) return;

    var helpText = HelpText.AutoBuild(_parserResult, h => {
      h.AdditionalNewLineAfterOption = false;
      h.Heading = "ipk25chat-client";
      h.AutoHelp = false;
      h.AutoVersion = false;
      return h;
    }, e => e);

    Console.WriteLine(helpText);
  }
}
