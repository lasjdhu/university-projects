<?php

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;
use IPP\Core\Interface\{OutputWriter, InputReader};

class StringClass
{
    public static function addMethods(ClassPrimitive $class, OutputWriter $stdout, InputReader $stdin): void
    {
        $methods = [
            "read" => function () use ($stdin) {
                return $stdin->readString();
            },
            "print" => function (ObjectI $self) use ($stdout) {
                $stdout->writeString((string)$self->attributes['value']);
                return $self;
            },
            "equalTo:" => function (ObjectI $self, array $args) {
                $argValue = $args[0] instanceof ObjectI ? (string)$args[0]->attributes['value'] : (string)$args[0];
                $isEqual = (string)$self->attributes['value'] === $argValue;
                $boolClass = $self->program->getClass($isEqual ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "asString" => function (ObjectI $self) {
                return $self;
            },
            "asInteger" => function (ObjectI $self) {
                if (is_numeric($self->attributes['value'])) {
                    $intClass = $self->program->getClass("Integer");
                    $intObj = new ObjectI($intClass, $self->program);
                    $intObj->attributes['value'] = (int)$self->attributes['value'];
                    return $intObj;
                }
                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
            "concatenateWith:" => function (ObjectI $self, array $args) {
                if ($args[0] instanceof ObjectI && $args[0]->class->name === "String") {
                    $strClass = $self->program->getClass("String");
                    $strObj = new ObjectI($strClass, $self->program);
                    $strObj->attributes['value'] =
                        (string)$self->attributes['value'] . (string)$args[0]->attributes['value'];
                    return $strObj;
                }
                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
            "startsWith:endsBefore:" => function () {
                // Not implemented
                return null;
            },
            "isString" => function () {
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
