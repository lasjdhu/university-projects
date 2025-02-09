<?php

/**
 * @brief Instruction class module
 * @file Instructuin.php
 * @author Dmitrii Ivanushkin xivanu00
 */

require __DIR__."/Line.php";
require __DIR__."/Argument.php";

class Instruction{
    
    public $opcode;
    public $args;

    public function __construct() {
        $this->args = [];
    }

    public function setOpcode($tokens) {
        $this->opcode = $tokens[0];
    }

    public function getOpcode() {
        return $this->opcode;
    }

    public function setArgs($token) {
        array_push($this->args, $token);
    }

    public function getArgs() {
        return $this->args;
    }

}


/**
 * @brief Abstract class (Factory design pattern)
 */
abstract class InstructionFactory {

    /**
     * @brief Fill in instruction based on input parameters
     * 
     * One line -> one instruction
     * Explodes the string, checks if first word is instruction,
     * and based on task specification creates exact number of arguments for it
     * 
     * @param Line $line
     */
    public static function createInstruction(Line $line): Instruction {
        $instruction = new Instruction();

        # Array of words in line
        $tokens = explode(" ", $line->content);
        $numOfArgs = count($tokens);

        # setOpcode will sets a name of instruction with a 0 element of tokens
        $instruction->setOpcode($tokens);

        # Group instrunction names based on specification
        $var_ = ["DEFVAR", "POPS"];
        $symb_ = ["PUSHS", "WRITE", "EXIT", "DPRINT"];
        $label_ = ["CALL", "LABEL", "JUMP"];
        $var_type_ = ["READ"];
        $var_symb_ = ["MOVE", "INT2CHAR", "STRLEN", "TYPE"];
        $var_symb_symb = ["ADD", "SUB", "MUL", "IDIV","LT", "GT", "EQ", 
        "AND", "OR", "NOT", "STRI2INT", "CONCAT", "GETCHAR", "SETCHAR"];
        $label_symb_symb = ["JUMPIFEQ", "JUMPIFNEQ"];
        $noArgs = ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"];

        $allArgs = array_merge($var_, $symb_, $label_, 
        $var_type_, $var_symb_, $var_symb_symb, $label_symb_symb, $noArgs);

        # Checks if instruction exists
        foreach ($allArgs as $arg) {
            # Instruction could be either capital or small, so I use strtoupper()
            if (strtoupper($instruction->opcode) == $arg) {
                if (in_array(strtoupper($instruction->opcode), $var_)) {
                    if ($numOfArgs != 2)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    # Push argument, created with its class Factory to an array
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "var"));
                }
                else if (in_array(strtoupper($instruction->opcode), $symb_)) {
                    if ($numOfArgs != 2)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "symb"));
                }
                else if (in_array(strtoupper($instruction->opcode), $label_)) {
                    if ($numOfArgs != 2)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "label"));
                }
                else if (in_array(strtoupper($instruction->opcode), $var_type_)) {
                    if ($numOfArgs != 3)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "var"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[2], "type"));
                }
                else if (in_array(strtoupper($instruction->opcode), $var_symb_)) {
                    if ($numOfArgs != 3)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "var"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[2], "symb"));
                }
                else if (in_array(strtoupper($instruction->opcode), $var_symb_symb)) {
                    if ($numOfArgs != 4)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "var"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[2], "symb"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[3], "symb"));
                }
                else if (in_array(strtoupper($instruction->opcode), $label_symb_symb)) {
                    if ($numOfArgs != 4)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[1], "label"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[2], "symb"));
                    $instruction->setArgs(ArgumentFactory::createArgument($tokens[3], "symb"));
                }
                else if (in_array(strtoupper($instruction->opcode), $noArgs)) {
                    if ($numOfArgs != 1)
                        exit(LEXICAL_OR_SYNTAX_ERR);
                }
            else 
                exit(OPERATION_ERR);
            }
        }

        return $instruction;
    }

}