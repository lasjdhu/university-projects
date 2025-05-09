<?php

/**
 * IPP - Block class definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Builtins;

use IPP\Core\ReturnCode;
use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Environment\ObjectI;

class BlockClass
{
    public static function addMethods(ClassPrimitive $class): void
    {
        $methods = [
            "whileTrue:" => function (ObjectI $self, array $args) {
                $conditionBlock = $self;
                $actionBlock = $args[0] ?? null;

                while (true) {
                    $conditionResult = null;
                    $conditionResult = $conditionBlock->sendMessage("value", []);

                    $isTruthy = $conditionResult instanceof ObjectI &&
                                $conditionResult->class->inherits("True", $self->program);

                    if (!$isTruthy) {
                        break;
                    }

                    if ($actionBlock instanceof ObjectI) {
                        $actionBlock->sendMessage("value", []);
                    } elseif (is_object($actionBlock) && method_exists($actionBlock, 'call')) {
                        $actionBlock->call([]);
                    }
                }

                $nilClass = $self->program->getClass("Nil");
                return new ObjectI($nilClass, $self->program);
            },
            "isBlock" => function (ObjectI $self) {
                $boolClass = $self->program->getClass("True");
                return new ObjectI($boolClass, $self->program);
            },
            // Wildcard method to handle all value:... methods
            "valueWildcard" => function (ObjectI $self, array $args, string $selector) {
                $arity = substr_count($selector, ':');
                $block = $self->attributes['block'] ?? null;

                if ($block->arity !== $arity) {
                    $self->program->stderr->writeString("Not expected block arity");
                    exit(ReturnCode::INTERPRET_DNU_ERROR);
                }
                return $block->call($args);
            }
        ];

        foreach ($methods as $selector => $implementation) {
            $method = new MethodPrimitive($selector);
            $method->implementation = $implementation;
            $class->methods[$selector] = $method;
        }
    }
}
