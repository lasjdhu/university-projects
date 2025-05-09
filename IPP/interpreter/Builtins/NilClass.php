<?php

/**
 * IPP - Nil class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;
use IPP\Student\Environment\TypeConverter;

class NilClass
{
    /** @var array<int, ObjectI> */
    private static array $instances = [];

    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "new" => function (ObjectI $self) {
                $programId = spl_object_id($self->program);
                if (!isset(self::$instances[$programId])) {
                    $nilClass = $self->program->getClass("Nil");
                    self::$instances[$programId] = new ObjectI($nilClass, $self->program);
                    self::$instances[$programId]->attributes['value'] = null;
                }
                return self::$instances[$programId];
            },
            "from:" => function (ObjectI $self) {
                return $self->sendMessage("new", []);
            },
            "asString" => function (ObjectI $self) {
                $converter = new TypeConverter($self->program);
                return $converter->toObject("nil");
            },
            "isNil" => function (ObjectI $self) {
                $converter = new TypeConverter($self->program);
                return $converter->toObject(true);
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
