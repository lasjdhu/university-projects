<?php

namespace IPP\Student\Environment;

use IPP\Core\ReturnCode;
use IPP\Student\Environment\ObjectI;
use IPP\Student\Program;
use IPP\Student\Primitives\BlockPrimitive;

class TypeConverter
{
    public Program $program;

    public function __construct(Program $program)
    {
        $this->program = $program;
    }

    /**
     * @param mixed $value
     * @return ObjectI
     */
    public function toObject($value): ObjectI
    {
        if ($value instanceof ObjectI) {
            return $value;
        }

        if (is_int($value)) {
            return $this->createInstance("Integer", $value);
        }

        if (is_string($value)) {
            return $this->createInstance("String", $value);
        }

        if (is_bool($value)) {
            return $this->createInstance($value ? "True" : "False", $value);
        }

        if ($value === null) {
            return $this->createInstance("Nil", null);
        }

        if (is_callable($value) && $value instanceof BlockPrimitive) {
            $instance = $this->createInstance("Block", $value);
            $instance->attributes['block'] = $value;
            return $instance;
        }

        $this->program->stderr->writeString("Type error: Cannot convert " . gettype($value) . " to object");
        exit(ReturnCode::INTERPRET_TYPE_ERROR);
    }

    /**
     * @param string $className
     * @param mixed $value
     * @return ObjectI
     */
    private function createInstance(string $className, $value): ObjectI
    {
        $class = $this->program->getClass($className);
        if (!$class) {
            $this->program->stderr->writeString("Type error: Class $className not found");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
        $instance = new ObjectI($class, $this->program);
        $instance->attributes['value'] = $value;
        return $instance;
    }
}
