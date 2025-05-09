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
     */
    public function sendMessage(string $selector, array $args): mixed
    {
        $method = $this->class->findMethod($selector, $this->program);
        if (!$method) {
            if (preg_match('/^[a-zA-Z][a-zA-Z0-9]*:?$/', $selector)) {
                if (substr($selector, -1) === ':') {
                    $attrName = substr($selector, 0, -1);

                    if (count($args) < 1) {
                        $this->program->stderr->writeString("Method not found");
                        exit(ReturnCode::INTERPRET_DNU_ERROR);
                    }

                    $this->attributes[$attrName] = $args[0];

                    return $this;
                } else {
                    if (isset($this->attributes[$selector])) {
                        return $this->attributes[$selector];
                    }
                }
            }

            $this->program->stderr->writeString("Method not found");
            exit(ReturnCode::INTERPRET_DNU_ERROR);
        }

        if ($method->selector === "valueWildcard") {
            return $method->invoke($this, $args, $selector);
        }

        return $method->invoke($this, $args);
    }
}
