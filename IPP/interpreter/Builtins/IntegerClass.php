<?php

namespace IPP\Student\Builtins;

use IPP\Core\ReturnCode;
use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;

class IntegerClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "equalTo:" => function (ObjectI $self, array $args) {
                $argValue = $args[0] instanceof ObjectI ? (int)$args[0]->attributes['value'] : (int)$args[0];
                $isEqual = (int)$self->attributes['value'] === $argValue;
                $boolClass = $self->program->getClass($isEqual ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "greaterThan:" => function (ObjectI $self, array $args) {
                $argValue = $args[0] instanceof ObjectI ? (int)$args[0]->attributes['value'] : (int)$args[0];
                $isGreater = (int)$self->attributes['value'] > $argValue;
                $boolClass = $self->program->getClass($isGreater ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "plus:" => function (ObjectI $self, array $args) {
                $argValue = $args[0] instanceof ObjectI ? (int)$args[0]->attributes['value'] : (int)$args[0];
                $result = (int)$self->attributes['value'] + $argValue;
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "minus:" => function (ObjectI $self, array $args) {
                $argValue = $args[0] instanceof ObjectI ? (int)$args[0]->attributes['value'] : (int)$args[0];
                $result = (int)$self->attributes['value'] - $argValue;
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "multiplyBy:" => function (ObjectI $self, array $args) {
                $result = (int)$self->attributes['value'] * (int)$args[0];
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "divBy:" => function (ObjectI $self, array $args) {
                if ((int)$args[0] === 0) {
                    $self->program->stderr->writeString("Division by zero\n");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }
                $result = intdiv((int)$self->attributes['value'], (int)$args[0]);
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "asString" => function (ObjectI $self) {
                $value = $self->attributes['value'];
                return ($value >= 0 ? '' : '-') . abs($value);
            },
            "asInteger" => function (ObjectI $self) {
                return $self;
            },
            "timesRepeat:" => function (ObjectI $self, array $args) {
                $result = null;
                for ($i = 1; $i <= (int)$self->attributes['value']; $i++) {
                    $result = $args[0]->call([$i]);
                }
                return $result;
            },
            "isNumber" => function (ObjectI $self) {
                return true;
            },
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
