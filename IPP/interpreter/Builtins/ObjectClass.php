<?php

/**
 * IPP - Object class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;

class ObjectClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "new" => function (ObjectI $self) {
                return new ObjectI($self->class, $self->program);
            },
            "from:" => function (ObjectI $self, array $args) {
                $instance = new ObjectI($self->class, $self->program);
                $instance->attributes['value'] = $args[0] ?? null;
                return $instance;
            },
            "identicalTo:" => function (ObjectI $self, array $args) {
                if (isset($args[0]) && $args[0] instanceof ObjectI) {
                    if (
                        $self->class->inherits("Nil", $self->program) &&
                        $args[0]->class->inherits("Nil", $self->program)
                    ) {
                        $boolClass = $self->program->getClass("True");
                        return new ObjectI($boolClass, $self->program);
                    }

                    if (
                        $self->class->inherits("True", $self->program) &&
                        $args[0]->class->inherits("True", $self->program)
                    ) {
                        $boolClass = $self->program->getClass("True");
                        return new ObjectI($boolClass, $self->program);
                    }

                    if (
                        $self->class->inherits("False", $self->program) &&
                        $args[0]->class->inherits("False", $self->program)
                    ) {
                        $boolClass = $self->program->getClass("True");
                        return new ObjectI($boolClass, $self->program);
                    }
                }

                $isIdentical = $self === $args[0];
                $boolClass = $self->program->getClass($isIdentical ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "equalTo:" => function (ObjectI $self, array $args) {
                if (!isset($args[0]) || !($args[0] instanceof ObjectI)) {
                    $boolClass = $self->program->getClass("False");
                    return new ObjectI($boolClass, $self->program);
                }

                if (
                    $self->class->inherits("Nil", $self->program) &&
                    $args[0]->class->inherits("Nil", $self->program)
                ) {
                    $boolClass = $self->program->getClass("True");
                    return new ObjectI($boolClass, $self->program);
                }

                if (
                    $self->class->inherits("True", $self->program) &&
                    $args[0]->class->inherits("True", $self->program)
                ) {
                    $boolClass = $self->program->getClass("True");
                    return new ObjectI($boolClass, $self->program);
                }

                if (
                    $self->class->inherits("False", $self->program) &&
                    $args[0]->class->inherits("False", $self->program)
                ) {
                    $boolClass = $self->program->getClass("True");
                    return new ObjectI($boolClass, $self->program);
                }

                if (empty($self->attributes) || empty($args[0]->attributes)) {
                    return $self->sendMessage("identicalTo:", [$args[0]]);
                }

                $selfValue = $self->attributes['value'] ?? null;
                $argValue = $args[0]->attributes['value'] ?? null;
                $isEqual = $selfValue === $argValue;

                $boolClass = $self->program->getClass($isEqual ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "asString" => function (ObjectI $self) {
                $stringClass = $self->program->getClass("String");
                $strObj = new ObjectI($stringClass, $self->program);
                $strObj->attributes['value'] = '';
                return $strObj;
            },
            "isNumber" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("False");
                return new ObjectI($boolClass, $self->program);
            },
            "isString" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("False");
                return new ObjectI($boolClass, $self->program);
            },
            "isBlock" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("False");
                return new ObjectI($boolClass, $self->program);
            },
            "isNil" => function (ObjectI $self) {
                $isNil = $self->class->inherits("Nil", $self->program);
                $boolClass = $self->program->getClass($isNil ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "ifTrue:ifFalse:" => function (ObjectI $self, array $args) {
                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        return $args[0]->sendMessage("value", []);
                    } elseif (is_object($args[0]) && method_exists($args[0], 'call')) {
                        return $args[0]->call([]);
                    }
                }

                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
