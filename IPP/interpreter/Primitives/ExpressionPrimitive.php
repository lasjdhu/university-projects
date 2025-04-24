<?php

/**
 * IPP - Expression definition
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Primitives;

use IPP\Core\ReturnCode;
use IPP\Student\Environment\ObjectI;
use IPP\Student\Environment\Execution;
use IPP\Student\Environment\TypeConverter;

class ExpressionPrimitive
{
    /** @var BlockPrimitive[] */
    public array $blocks = [];
    /** @var ExpressionPrimitive[] */
    public array $arguments = [];
    public int $argOrder = 0;
    public ?string $varName = null;
    public ?LiteralPrimitive $literal = null;
    public ?string $selector = null;
    private ?TypeConverter $typeConverter = null;

    /**
     * @param Execution $context
     * @return mixed
     */
    public function evaluate(Execution $context)
    {
        if ($this->typeConverter === null) {
            $this->typeConverter = new TypeConverter($context->self->program);
        }

        if ($this->literal !== null) {
            return $this->evaluateLiteral();
        }

        if ($this->varName !== null) {
            $value = $context->getVariable($this->varName);
            if ($value === null && $this->varName !== 'nil') {
                $context->self->program->stderr->writeString("Variable not found: {$this->varName}");
                exit(ReturnCode::INTERPRET_VALUE_ERROR);
            }
            return $value;
        }

        if ($this->selector !== null) {
            return $this->evaluateSend($context);
        }

        if (!empty($this->blocks)) {
            return $this->evaluateBlock($context);
        }

        return null;
    }

    /**
     * @return mixed
     */
    private function evaluateLiteral()
    {
        switch ($this->literal->className) {
            case 'Integer':
                if (!is_numeric($this->literal->value)) {
                    $this->typeConverter->program->stderr->writeString("Value error: Invalid integer literal: {$this->literal->value}");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }
                return (int)$this->literal->value;
            case 'String':
                return (string)$this->literal->value;
            case 'True':
                return true;
            case 'False':
                return false;
            case 'Nil':
                return null;
            case 'class':
                return $this->literal->value;
            default:
            $this->typeConverter->program->stderr->writeString("Type error: Unknown literal type: {$this->literal->className}");
                exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
    }

    /**
     * @param Execution $context
     * @return mixed
     */
    private function evaluateSend(Execution $context)
    {
        $args = $this->evaluateArguments($context);

        if ($this->selector === 'from:' && is_string($args[0])) {
            return $this->createClassInstance($args[0], array_slice($args, 1), $context);
        }

        if (empty($args)) {
            return null;
        }

        try {
            $receiver = $this->typeConverter->toObject($args[0]);
            return $receiver->sendMessage($this->selector, array_slice($args, 1));
        } catch (\Throwable $e) {
            $context->self->program->stderr->writeString("Type error: Failed to convert receiver for message: {$this->selector}");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
    }

    /**
     * @param Execution $context
     * @return mixed[]
     */
    private function evaluateArguments(Execution $context): array
    {
        usort($this->arguments, fn($a, $b) => $a->argOrder <=> $b->argOrder);
        return array_map(fn($expr) => $expr->evaluate($context), $this->arguments);
    }

    /**
     * @param string $className
     * @param mixed[] $args
     * @param Execution $context
     */
    private function createClassInstance(string $className, array $args, Execution $context): ObjectI
    {
        $class = $context->self->program->getClass($className);
        if (!$class) {
            $context->self->program->stderr->writeString("Type error: Class not found: $className");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
        
        $instance = new ObjectI($class, $context->self->program);
        $instance->attributes['value'] = $args[0] ?? null;
        return $instance;
    }

    // creates a closure by binding the block to the current context
    private function evaluateBlock(Execution $outer): callable
    {
        if (empty($this->blocks)) {
            $outer->self->program->stderr->writeString("Value error: Empty block reference");
            exit(ReturnCode::INTERPRET_VALUE_ERROR);
        }
        
        $block = $this->blocks[0];
        return $block->bindContext($outer);
    }
}
