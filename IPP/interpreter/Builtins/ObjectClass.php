<?php

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
                $instance->attributes['value'] = $args[0];
                return $instance;
            },
            "identicalTo:" => function (ObjectI $self, array $args) {
                $isIdentical = $self === $args[0];
                $boolClass = $self->program->getClass($isIdentical ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "equalTo:" => function (ObjectI $self, array $args) {
                if (empty($self->attributes) || empty($args[0]->attributes)) {
                    return $self->sendMessage("identicalTo:", [$args[0]]);
                }
                $isEqual = $self->attributes['value'] === $args[0]->attributes['value'];
                $boolClass = $self->program->getClass($isEqual ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "asString" => function () {
                return '';
            },
            "isNumber" => function () {
                return false;
            },
            "isString" => function () {
                return false;
            },
            "isBlock" => function () {
                return false;
            },
            "isNil" => function () {
                return false;
            },
            "attr:" => function (ObjectI $self, array $args) {
                $self->attributes['value'] = $args[0];
                return $self;
            },
            "attr" => function (ObjectI $self) {
                return $self->attributes['value'] ?? null;
            },
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
