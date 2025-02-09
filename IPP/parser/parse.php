<?php

/**
 * @brief Main skript k 1. úloze do IPP 2022/2023
 * @file parse.php
 * @author Dmitrii Ivanushkin xivanu00
 */

require __DIR__."/Program.php";
require __DIR__."/Output.php";

ini_set('display_errors', 'stderr');

#DEFINE all error codes
const PARAM_ERR = 10;
const INPUT_ERR = 11;
const OUTPUT_ERR = 12;
const HEADER_ERR = 21;
const OPERATION_ERR = 22;
const LEXICAL_OR_SYNTAX_ERR = 23;
const INTER_ERR = 99;

/**
 * @brief Prints help
 */
function printHelp() {
    echo "Usage: <interpreter> parse.php [--help]\n\n";

    echo "Skript typu filtr nacte ze standardniho vstupu zdrojovy kod v IPP-code23,\n";
    echo "zkontroluje lexikalni a syntaktickou spravnost kodu\n";
    echo "a vypise na standardni vystup XML reprezentaci programu\n\n";

    echo "Chybove navratove kody:\n";
    echo "    10 - chybejici parametr skriptu nebo pouziti zakazane kombinace parametru\n";
    echo "    11 - chyba pri otevirani vstupnich souboru\n";
    echo "    12 - chyba pri otevreni vystupnich souboru pro zapis\n";
    echo "    21 - chybna nebo chybejici hlavicka ve zdrojovem kodu zapsanem v IPPcode23\n";
    echo "    22 - neznamy nebo chybny operacni kod ve zdrojovem kodu zapsanem v IPPcode23\n";
    echo "    23 - jina lexikalni nebo syntakticka chyba zdrojoveho kodu zapsaneho v IPPcode23\n";
    echo "    99 - interni chyba\n";
}

/**
 * @brief Checks if [-h] flag is enabled and if nothing else is specified
 */
function hasHelp() {
    global $argc, $argv;

    if (isset($argc)) {
        if ($argc == 2 && $argv[1] == "--help") {
            printHelp();
            exit(0);
        }
        else if ($argc > 2 || (isset($argv[1]) && $argv[1] != "--help"))
            exit(PARAM_ERR);
    }
}

function main() {
    hasHelp();

    # Create new layout for input data
    $in = new Program();

    # Read till the EOF, delete comments in strings,
    # check for Header and fill the Program with instructions  
    while (!feof(STDIN)) {
        $line = new Line();
        $line->deleteComments();

        if (!empty($line->content)) {
            if ($line->content == ".IPPcode23")
                $in->setHeader();
            else {
                $instruction = InstructionFactory::createInstruction($line);
                $in->setInstruction($instruction);
            }

            if (!$in->getHeader())
                exit(HEADER_ERR);
        }
    }

    # Print result
    $out = new Output($in);
    $out->printObj();

    exit(0);
}

main();

?>