<?php

/**
 * IPP - Statement definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Environment\Execution;

class StatementPrimitive
{
    public int $order;
    public ?string $varName = null;
    /** @var ExpressionPrimitive[] */
    public array $expressions = [];

    public function __construct(int $order)
    {
        $this->order = $order;
    }

    /**
     * @param Execution $context
     * @return mixed
     */
    public function execute(Execution $context)
    {
        $result = null;
        foreach ($this->expressions as $expr) {
            $result = $expr->evaluate($context);
        }

        if ($this->varName !== null) {
            $context->setVariable($this->varName, $result);
        }

        return $result;
    }
}
