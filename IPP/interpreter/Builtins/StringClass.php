<?php

/**
 * IPP - String class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

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
                $value = $self->attributes['value'] ?? '';
                $stdout->writeString((string)$value);
                return $self;
            },
            "equalTo:" => function (ObjectI $self, array $args) {
                $selfValue = $self->attributes['value'] ?? '';

                $argValue = '';
                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        $argValue = $args[0]->attributes['value'] ?? '';
                    } else {
                        $argValue = (string)$args[0];
                    }
                }

                $boolClass = $self->program->getClass($selfValue === $argValue ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "asString" => function (ObjectI $self) {
                return $self;
            },
            "asInteger" => function (ObjectI $self) {
                $value = $self->attributes['value'] ?? '';
                if (is_numeric($value)) {
                    $intClass = $self->program->getClass("Integer");
                    $intObj = new ObjectI($intClass, $self->program);
                    $intObj->attributes['value'] = (int)$value;
                    return $intObj;
                }
                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
            "concatenateWith:" => function (ObjectI $self, array $args) {
                if (
                    isset($args[0]) && $args[0] instanceof ObjectI &&
                    $args[0]->class->inherits("String", $self->program)
                ) {
                    $selfValue = $self->attributes['value'] ?? '';
                    $argValue = $args[0]->attributes['value'] ?? '';

                    $strClass = $self->program->getClass("String");
                    $strObj = new ObjectI($strClass, $self->program);
                    $strObj->attributes['value'] = $selfValue . $argValue;
                    return $strObj;
                }
                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
            "startsWith:endsBefore:" => function (ObjectI $self, array $args) {
                $str = $self->attributes['value'] ?? '';

                $start = 0;
                $end = 0;

                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        $start = (int)($args[0]->attributes['value'] ?? 0);
                    } else {
                        $start = (int)$args[0];
                    }
                }

                if (isset($args[1])) {
                    if ($args[1] instanceof ObjectI) {
                        $end = (int)($args[1]->attributes['value'] ?? 0);
                    } else {
                        $end = (int)$args[1];
                    }
                }

                if ($start <= 0 || $end <= 0) {
                    $nilClass = $self->program->getClass("Nil");
                    return new ObjectI($nilClass, $self->program);
                }

                if ($end <= $start) {
                    $stringClass = $self->program->getClass("String");
                    $strObj = new ObjectI($stringClass, $self->program);
                    $strObj->attributes['value'] = '';
                    return $strObj;
                }

                $start--;
                $end--;

                $result = substr($str, $start, $end - $start);

                $stringClass = $self->program->getClass("String");
                $strObj = new ObjectI($stringClass, $self->program);
                $strObj->attributes['value'] = $result;
                return $strObj;
            },
            "isString" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("True");
                return new ObjectI($boolClass, $self->program);
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
