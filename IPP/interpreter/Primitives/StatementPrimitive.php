<?php

/**
 * IPP - Statement primitive
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Environment\Execution;
use IPP\Student\Environment\TypeConverter;

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

    public function execute(Execution $context): mixed
    {
        $result = null;
        foreach ($this->expressions as $expr) {
            $result = $expr->evaluate($context);
        }

        if ($this->varName !== null) {
            $context->setVariable($this->varName, $result);
        }

        if ($result === null) {
            $converter = new TypeConverter($context->self->program);
            return $converter->toObject(null);
        }

        return $result;
    }
}
