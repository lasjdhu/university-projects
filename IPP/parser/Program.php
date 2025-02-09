<?php

/**
 * @brief Program (layout) class module
 * @file Program.php
 * @author Dmitrii Ivanushkin xivanu00
 */

require __DIR__."/Instruction.php";

class Program{
    
    private $header;
    private $instructions;

    public function __construct() {
        $this->header = false;
        $this->instructions = [];
    }

    public function setHeader() {
        $this->header = true;
    }

    public function getHeader() {
        return $this->header;
    }

    public function setInstruction(Instruction $instruction) {
        $this->instructions[] = $instruction;
    }

    public function getInstructions() {
        return $this->instructions;
    }

}

?>
