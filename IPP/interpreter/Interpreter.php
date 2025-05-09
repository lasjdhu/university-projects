<?php

/**
 * IPP - SOL25 Interpreter
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Core\ReturnCode;

class Interpreter extends AbstractInterpreter
{
    public function execute(): int
    {
        $dom = $this->source->getDOMDocument();

        $program = new Program($this->stdout, $this->input, $this->stderr);
        $parser = new Parser($dom);

        $parser->parse($program);
        $program->execute();

        return ReturnCode::OK;
    }
}
