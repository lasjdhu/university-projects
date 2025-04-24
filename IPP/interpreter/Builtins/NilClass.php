<?php

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;

class NilClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "asString" => function () {
                return "nil";
            },
            "isNil" => function () {
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
