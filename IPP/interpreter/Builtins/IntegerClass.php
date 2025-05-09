<?php

/**
 * IPP - Integer class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Builtins;

use IPP\Core\ReturnCode;
use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;

class IntegerClass
{
    /**
     * @param mixed $value
     */
    private static function safeIntValue($value): int
    {
        if ($value instanceof ObjectI) {
            if (isset($value->attributes['value'])) {
                return self::safeIntValue($value->attributes['value']);
            }
            return 0;
        }
        return (int)$value;
    }

    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "equalTo:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                $argValue = 0;
                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        if ($args[0]->class->inherits("Integer", $self->program)) {
                            $argValue = isset($args[0]->attributes['value'])
                                ? self::safeIntValue($args[0]->attributes['value'])
                                : 0;
                        } else {
                            $boolClass = $self->program->getClass("False");
                            return new ObjectI($boolClass, $self->program);
                        }
                    } else {
                        $argValue = (int)$args[0];
                    }
                }

                $isEqual = $selfValue === $argValue;
                $boolClass = $self->program->getClass($isEqual ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "greaterThan:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                $argValue = 0;
                if (isset($args[0])) {
                    if ($args[0] instanceof ObjectI) {
                        if ($args[0]->class->inherits("Integer", $self->program)) {
                            $argValue = isset($args[0]->attributes['value'])
                                ? self::safeIntValue($args[0]->attributes['value'])
                                : 0;
                        } else {
                            $boolClass = $self->program->getClass("False");
                            return new ObjectI($boolClass, $self->program);
                        }
                    } else {
                        $argValue = (int)$args[0];
                    }
                }

                $isGreater = $selfValue > $argValue;
                $boolClass = $self->program->getClass($isGreater ? "True" : "False");
                return new ObjectI($boolClass, $self->program);
            },
            "plus:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                if (!isset($args[0])) {
                    $self->program->stderr->writeString("Missing argument for plus:");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }

                if ($args[0] instanceof ObjectI) {
                    if (!$args[0]->class->inherits("Integer", $self->program)) {
                        $self->program->stderr->writeString("Cannot add");
                        exit(ReturnCode::INTERPRET_VALUE_ERROR);
                    }
                    $argValue = isset($args[0]->attributes['value'])
                        ? self::safeIntValue($args[0]->attributes['value'])
                        : 0;
                } elseif (!is_numeric($args[0])) {
                    $self->program->stderr->writeString("Cannot add");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                } else {
                    $argValue = (int)$args[0];
                }

                $result = $selfValue + $argValue;
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "minus:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                if (!isset($args[0])) {
                    $self->program->stderr->writeString("Missing argument for minus:");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }

                if ($args[0] instanceof ObjectI) {
                    if (!$args[0]->class->inherits("Integer", $self->program)) {
                        $self->program->stderr->writeString("Cannot subtract");
                        exit(ReturnCode::INTERPRET_VALUE_ERROR);
                    }
                    $argValue = isset($args[0]->attributes['value'])
                        ? self::safeIntValue($args[0]->attributes['value'])
                        : 0;
                } elseif (!is_numeric($args[0])) {
                    $self->program->stderr->writeString("Cannot subtract");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                } else {
                    $argValue = (int)$args[0];
                }

                $result = $selfValue - $argValue;
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "multiplyBy:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                if (!isset($args[0])) {
                    $self->program->stderr->writeString("Missing argument for multiplyBy:");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }

                if ($args[0] instanceof ObjectI) {
                    if (!$args[0]->class->inherits("Integer", $self->program)) {
                        $self->program->stderr->writeString("Cannot multiply");
                        exit(ReturnCode::INTERPRET_VALUE_ERROR);
                    }
                    $argValue = isset($args[0]->attributes['value'])
                        ? self::safeIntValue($args[0]->attributes['value'])
                        : 0;
                } elseif (!is_numeric($args[0])) {
                    $self->program->stderr->writeString("Cannot multiply");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                } else {
                    $argValue = (int)$args[0];
                }

                $result = $selfValue * $argValue;
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "divBy:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;

                if (!isset($args[0])) {
                    $self->program->stderr->writeString("Missing argument for divBy:");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }

                $argValue = 0;
                if ($args[0] instanceof ObjectI) {
                    if (!$args[0]->class->inherits("Integer", $self->program)) {
                        $self->program->stderr->writeString("Cannot divide");
                        exit(ReturnCode::INTERPRET_VALUE_ERROR);
                    }
                    $argValue = isset($args[0]->attributes['value'])
                        ? self::safeIntValue($args[0]->attributes['value'])
                        : 0;
                } elseif (!is_numeric($args[0])) {
                    $self->program->stderr->writeString("Cannot divide");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                } else {
                    $argValue = (int)$args[0];
                }

                if ($argValue === 0) {
                    $self->program->stderr->writeString("Division by zero");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }

                $result = intdiv($selfValue, $argValue);
                $intClass = $self->program->getClass("Integer");
                $intObj = new ObjectI($intClass, $self->program);
                $intObj->attributes['value'] = $result;
                return $intObj;
            },
            "asString" => function (ObjectI $self) {
                $value = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;
                $stringValue = ($value >= 0 ? '' : '-') . abs($value);
                $stringClass = $self->program->getClass("String");
                $strObj = new ObjectI($stringClass, $self->program);
                $strObj->attributes['value'] = $stringValue;
                return $strObj;
            },
            "asInteger" => function (ObjectI $self) {
                return $self;
            },
            "timesRepeat:" => function (ObjectI $self, array $args) {
                $selfValue = isset($self->attributes['value'])
                    ? self::safeIntValue($self->attributes['value'])
                    : 0;
                $result = null;

                if (isset($args[0])) {
                    $block = $args[0];
                    for ($i = 1; $i <= $selfValue; $i++) {
                        $intClass = $self->program->getClass("Integer");
                        $intObj = new ObjectI($intClass, $self->program);
                        $intObj->attributes['value'] = $i;

                        if ($block instanceof ObjectI) {
                            $result = $block->sendMessage("value:", [$intObj]);
                        } elseif (is_object($block) && method_exists($block, 'call')) {
                            $result = $block->call([$intObj]);
                        }
                    }
                }

                if ($result === null) {
                    $nilClass = $self->program->getClass("Nil");
                    return new ObjectI($nilClass, $self->program);
                }

                return $result;
            },
            "isNumber" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("True");
                return new ObjectI($boolClass, $self->program);
            },
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
