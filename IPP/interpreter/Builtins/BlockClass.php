<?php

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;

class BlockClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "whileTrue:" => function () {
                // Not implemented
                return null;
            },
            "isBlock" => function () {
                return true;
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
