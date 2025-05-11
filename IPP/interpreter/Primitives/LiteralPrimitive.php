<?php

/**
 * IPP - Literal primitive
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

class LiteralPrimitive
{
    public string $className;
    public string $value;

    public function __construct(string $className, string $value)
    {
        $this->className = $className;
        $this->value = $value;
    }
}
