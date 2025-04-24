<?php

/**
 * IPP - Class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Program;

class ClassPrimitive
{
    public string $name;
    public ?string $parent;
    /** @var MethodPrimitive[] */
    public array $methods = [];

    public function __construct(string $name, ?string $parent)
    {
        $this->name = $name;
        $this->parent = $parent;
    }

    public function findMethod(string $selector, Program $program): ?MethodPrimitive
    {
        foreach ($this->methods as $method) {
            if ($method->selector === $selector) {
                return $method;
            }
        }

        if ($this->parent) {
            $parent = $program->getClass($this->parent);
            if ($parent) {
                return $parent->findMethod($selector, $program);
            }
        }

        return null;
    }
}
