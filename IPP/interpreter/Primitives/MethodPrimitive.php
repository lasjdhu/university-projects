<?php

/**
 * IPP - Method definition
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
     */
    public function invoke(ObjectI $receiver, array $args): mixed
    {
        if ($this->implementation !== null) {
            return ($this->implementation)($receiver, $args);
        }

        // methods use their first block with receiver as self for execution
        $block = $this->blocks[0];
        $context = new Execution($receiver, $args, $block->parameters ?? []);
        return $block->execute($context);
    }
}
