<?php

/**
 * @brief Output (ready XML) class module
 * @file Output.php
 * @author Dmitrii Ivanushkin xivanu00
 */

class Output {

    private $program;

    /**
     * @param Program $program
     */
    public function __construct(Program $program) {
        $this->program = $program;
    }

    /**
     * @brief PRIVATE Translate XML characters and print argument layout on output
     * @param array $arrofArgs
     */
    private function printArgs($arrOfArgs) {
        $cnt= count($arrOfArgs);

        if ($cnt != 0) {
            $string_regex = "/[<>&]/";
            
            if (($arrOfArgs[0]->type == "string") &&
            (preg_match($string_regex, $arrOfArgs[0]->val))) {
                # List of translateable characters
                $entities = [
                    ">" => "&gt;",
                    "<" => "&lt;",
                    "&" => "&amp;"
                ];

                # Translating values using preg_replace_callback() function
                $arrOfArgs[0]->val = preg_replace_callback($string_regex, 
                    function ($match) use ($entities) {
                        return $entities[$match[0]];
                    }
                , $arrOfArgs[0]->val);
            }
        }

        # Print exact number of arguments in instruction 
        # (based on array in Argument class)
        switch ($cnt) {
            case 0:
                echo "";
                break;
            case 1:
                echo "  <arg1 type=\"".$arrOfArgs[0]->type."\">";
                echo $arrOfArgs[0]->val."</arg1>\n";
                break;
            case 2:
                echo "  <arg1 type=\"".$arrOfArgs[0]->type."\">";
                echo $arrOfArgs[0]->val."</arg1>\n";
                
                echo "  <arg2 type=\"".$arrOfArgs[1]->type."\">";
                echo $arrOfArgs[1]->val."</arg2>\n";
                break;
            case 3:
                echo "  <arg1 type=\"".$arrOfArgs[0]->type."\">";
                echo $arrOfArgs[0]->val."</arg1>\n";
                
                echo "  <arg2 type=\"".$arrOfArgs[1]->type."\">";
                echo $arrOfArgs[1]->val."</arg2>\n";

                echo "  <arg3 type=\"".$arrOfArgs[2]->type."\">";
                echo $arrOfArgs[2]->val."</arg3>\n";
                break;
            default:
                exit(INTER_ERR);
        }
    }

    /**
     * @brief PRIVATE Print instructions and call function to print arguments
     */
    private function printInstructions() {
        $arrOfInstrs = $this->program->getInstructions();
        foreach ($arrOfInstrs as $i => $e) {
            echo " <instruction order=\"". $i + 1 ."\" opcode=\"".$e->opcode."\">\n";
            $this->printArgs($e->getArgs());
            echo " </instruction>\n";
        }
    }

    /**
     * @brief Print result
     */
    public function printObj() {
        echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        echo "<program language=\"IPPcode23\">\n";
        $this->printInstructions();
        echo "</program>\n";
    }
    
}

?>