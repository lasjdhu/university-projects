<?php

/**
 * IPP - Boolean class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Builtins;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;
use IPP\Student\Environment\TypeConverter;
use IPP\Student\Primitives\BlockPrimitive;

class BooleanClass
{
    /** @var array<int, ObjectI> */
    private static array $trueInstances = [];
    /** @var array<int, ObjectI> */
    private static array $falseInstances = [];

    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "new" => function (ObjectI $self) {
                $programId = spl_object_id($self->program);

                if ($self->class->name === "True") {
                    if (!isset(self::$trueInstances[$programId])) {
                        self::$trueInstances[$programId] = new ObjectI($self->class, $self->program);
                    }
                    return self::$trueInstances[$programId];
                } elseif ($self->class->name === "False") {
                    if (!isset(self::$falseInstances[$programId])) {
                        self::$falseInstances[$programId] = new ObjectI($self->class, $self->program);
                    }
                    return self::$falseInstances[$programId];
                }

                return new ObjectI($self->class, $self->program);
            },
            "from:" => function (ObjectI $self) {
                return $self->sendMessage("new", []);
            },
            "not" => function (ObjectI $self) {
                $isTrue = $self->class->inherits("True", $self->program);
                $converter = new TypeConverter($self->program);
                return $converter->toObject(!$isTrue);
            },
            "and:" => function (ObjectI $self, array $args) {
                $isTrue = $self->class->inherits("True", $self->program);

                if (!$isTrue) {
                    $converter = new TypeConverter($self->program);
                    return $converter->toObject(false);
                }

                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        $result = $args[0]->sendMessage("value", []);

                        if (
                            $result instanceof ObjectI &&
                            ($result->class->inherits("True", $self->program) ||
                             $result->class->inherits("False", $self->program))
                        ) {
                            return $result;
                        }

                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(false);
                    } elseif (is_object($args[0]) && method_exists($args[0], 'call')) {
                        $result = $args[0]->call([]);

                        if (
                            $result instanceof ObjectI &&
                            ($result->class->inherits("True", $self->program) ||
                             $result->class->inherits("False", $self->program))
                        ) {
                            return $result;
                        }

                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(false);
                    } else {
                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(false);
                    }
                }

                $converter = new TypeConverter($self->program);
                return $converter->toObject(false);
            },
            "or:" => function (ObjectI $self, array $args) {
                $isTrue = $self->class->inherits("True", $self->program);

                if ($isTrue) {
                    return $self;
                }

                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        $result = $args[0]->sendMessage("value", []);

                        if (
                            $result instanceof ObjectI &&
                            ($result->class->inherits("True", $self->program) ||
                             $result->class->inherits("False", $self->program))
                        ) {
                            return $result;
                        }

                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(true);
                    } elseif (is_object($args[0]) && method_exists($args[0], 'call')) {
                        $result = $args[0]->call([]);

                        if (
                            $result instanceof ObjectI &&
                            ($result->class->inherits("True", $self->program) ||
                             $result->class->inherits("False", $self->program))
                        ) {
                            return $result;
                        }

                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(true);
                    } else {
                        $converter = new TypeConverter($self->program);
                        return $converter->toObject(false);
                    }
                }

                $converter = new TypeConverter($self->program);
                return $converter->toObject(false);
            },
            "ifTrue:ifFalse:" => function (ObjectI $self, array $args) {
                $result = null;
                $index = $self->class->inherits("True", $self->program) ? 0 : 1;

                if (isset($args[$index])) {
                    $arg = $args[$index];

                    if ($arg instanceof ObjectI) {
                        $result = $arg->sendMessage("value", []);
                    } elseif ($arg instanceof BlockPrimitive) {
                        $result = $arg->call([]);
                    }
                }

                if ($result === null) {
                    $nilClass = $self->program->getClass("Nil");
                    return new ObjectI($nilClass, $self->program);
                }

                return $result;
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
