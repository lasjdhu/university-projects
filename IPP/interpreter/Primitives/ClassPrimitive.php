<?php

/**
 * IPP - Class primitive
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

        if ($this->name === "Block" && strpos($selector, "value") === 0) {
            if (isset($this->methods["valueWildcard"])) {
                return $this->methods["valueWildcard"];
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

    public function inherits(string $className, Program $program): bool
    {
        if ($this->name === $className) {
            return true;
        }

        if ($this->parent) {
            $parent = $program->getClass($this->parent);
            if ($parent) {
                return $parent->inherits($className, $program);
            }
        }

        return false;
    }
}
