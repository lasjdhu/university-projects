<?php

/**
 * IPP - Object structure
 * @author Dmitrii Ivanushkin (xivanu00)
 */

 namespace IPP\Student\Environment;

use IPP\Core\ReturnCode;
use IPP\Student\Program;
 use IPP\Student\Primitives\ClassPrimitive;

class ObjectI
{
    public ClassPrimitive $class;
    /** @var mixed[] */
    public array $attributes = [];
    public Program $program;

    public function __construct(ClassPrimitive $class, Program $program)
    {
        $this->class = $class;
        $this->program = $program;
    }

    /**
     * @param string $selector
     * @param mixed[] $args
     * @return mixed
     */
    public function sendMessage(string $selector, array $args)
    {
        // message dispatching - follows inheritance chain to find appropriate method
        $method = $this->class->findMethod($selector, $this->program);
        if (!$method) {
            $this->program->stderr->writeString("Method not found: $selector");
            exit(ReturnCode::INTERPRET_DNU_ERROR);
        }
        return $method->invoke($this, $args);
    }
}
