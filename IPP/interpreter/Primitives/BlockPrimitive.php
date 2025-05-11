<?php

/**
 * IPP - Block primitive
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Environment\Execution;
use IPP\Student\Environment\TypeConverter;

class BlockPrimitive
{
    public int $arity;
    /** @var string[] */
    public array $parameters = [];
    /** @var StatementPrimitive[] */
    public array $statements = [];
    private ?Execution $parentContext = null;
    private ?Execution $outerContext = null;

    public function __construct(int $arity)
    {
        $this->arity = $arity;
    }

    // contextualizes environment
    public function bindContext(Execution $outer): self
    {
        $this->outerContext = $outer;
        return $this;
    }

    /**
     * @param mixed ...$args
     */
    public function __invoke(...$args): mixed
    {
        // invoking block creates a new execution context with the outer self reference
        $ctx = new Execution($this->outerContext->self, $args, $this->parameters ?? []);
        return $this->execute($ctx);
    }

    /**
     * @param mixed[] $args
     */
    public function call(array $args): mixed
    {
        return $this->__invoke(...$args);
    }

    /**
     * @param Execution $context
     */
    public function execute(Execution $context): mixed
    {
        try {
            $previousParent = $this->parentContext;
            $this->parentContext = $context;

            foreach ($this->parameters as $index => $param) {
                $context->setVariable($param, $context->arguments[$index]);
            }

            // sort statements by their order attribute
            $sortedStatements = $this->statements;
            usort($sortedStatements, function ($a, $b) {
                return $a->order <=> $b->order;
            });

            // block returns the value of the last statement
            $result = null;
            foreach ($sortedStatements as $statement) {
                $result = $statement->execute($context);
            }

            if ($result === null) {
                $converter = new TypeConverter($context->self->program);
                return $converter->toObject(null);
            }

            return $result;
        } finally {
            // restore previous parent context when execution finishes
            $this->parentContext = $previousParent;
        }
    }

    public function addStatement(StatementPrimitive $statement): void
    {
        $this->statements[] = $statement;
    }

    /**
     * @param string[] $parameters
     */
    public function setParameters(array $parameters): void
    {
        $this->parameters = $parameters;
    }
}
