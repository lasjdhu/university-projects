<?php

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;

class BooleanClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "not" => function () {
                // Not implemented
                return null;
            },
            "and:" => function () {
                // Not implemented
                return null;
            },
            "or:" => function () {
                // Not implemented
                return null;
            },
            "ifTrue:ifFalse:" => function (ObjectI $self, array $args) {
                if ($self->class->name === "True") {
                    return $args[0]->call([]);
                } else {
                    return $args[1]->call([]);
                }
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
