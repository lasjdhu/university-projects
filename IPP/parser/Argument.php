<?php

/**
 * @brief Argument class module
 * @file Argument.php
 * @author Dmitrii Ivanushkin xivanu00
 */

class Argument {

    public $type;
    public $val;

    /**
     * @param string $arg
     */
    public function __construct($arg) {    
        $this->val = $arg;
    }

    public function setType($str) {
        $this->type = $str;
    }

    public function getType() {
        return $this->type;
    }

    public function setVal($val) {
        $this->val = $val;
    }

    public function getVal() {
        return $this->type;
    }

}

/**
 * @brief Abstract class (Factory design pattern)
 */
abstract class ArgumentFactory {

    /**
     * @brief Fill in argument type and value based on input parameters
     * 
     * @param string $token
     * @param string $type
     */
    public static function createArgument($token, $type): Argument {
        # Regular Expressions
        $var_regex = "/(LF|TF|GF)@[a-zA-Z_$&%*!?-][a-zA-Z0-9_$&%*!?-]*/";
        $symb_regex = "/string@([^\s\\]|\\\\[0-9]{3})*|bool@(true|false)|int@[-+]?[0-9]+|nil@nil/";
        $label_regex = "/[a-zA-Z_$&%*!?-][a-zA-Z0-9_$&%*!?-]*/";

        # For confidence
        if ($token == null)
            exit(LEXICAL_OR_SYNTAX_ERR);
        
        # Create new Objet and checks of its content goes to one of groups, defined by RegEx
        $arg = new Argument($token);
        if (preg_match("/@/", $token)) {
            if ((preg_match($symb_regex, $token)) && ($type == "symb")) {
                $pos = strpos($token, "@");
                # Divide by @ and left part goes to a type, and right to a value
                $arg->setType(substr($token, 0, $pos));
                $arg->setVal(substr($token, $pos + 1));
            }
            else if ((preg_match($var_regex, $token)) && (($type == "var") || ($type == "symb")))
                $arg->setType("var");
            else exit(LEXICAL_OR_SYNTAX_ERR);
        }
        else if ((preg_match($label_regex, $token)) && ($type == "label")){
            $arg->setType("label");
        }
        else if ($type == "type") {
            $arg->setType("type");
            $arg->setVal($token);
        }
        else exit(LEXICAL_OR_SYNTAX_ERR);

        return $arg;
    }
    
}

?>