<?php

/**
 * IPP - Block definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Student\Environment\Execution;

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

            // block returns the value of the last statement
            $result = null;
            foreach ($this->statements as $statement) {
                $result = $statement->execute($context);
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
