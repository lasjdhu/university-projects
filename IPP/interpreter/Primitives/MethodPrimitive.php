<?php

/**
 * IPP - Method primitive
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Environment\ObjectI;
use IPP\Student\Environment\Execution;

class MethodPrimitive
{
    public string $selector;
    /** @var BlockPrimitive[] */
    public array $blocks = [];
    /** @var callable|null */
    public $implementation = null;

    public function __construct(string $selector)
    {
        $this->selector = $selector;
    }

    /**
     * @param ObjectI $receiver
     * @param mixed[] $args
     * @param string|null $originalSelector
     */
    public function invoke(ObjectI $receiver, array $args, ?string $originalSelector = null): mixed
    {
        if ($this->implementation !== null) {
            if ($this->selector === "valueWildcard" && $originalSelector !== null) {
                return ($this->implementation)($receiver, $args, $originalSelector);
            }
            return ($this->implementation)($receiver, $args);
        }

        $block = $this->blocks[0];
        $context = new Execution($receiver, $args, $block->parameters ?? []);

        if ($receiver->class->parent) {
            $parentClass = $receiver->program->getClass($receiver->class->parent);
            if ($parentClass) {
                $superO = new ObjectI($parentClass, $receiver->program);
                $superO->attributes = $receiver->attributes;
                $context->setVariable('super', $superO);
            }
        }

        return $block->execute($context);
    }
}
