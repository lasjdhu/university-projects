<?php

/**
 * IPP - Expression primitive
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

    public function evaluate(Execution $context): mixed
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
                $context->self->program->stderr->writeString("Variable not found");
                exit(ReturnCode::INTERPRET_VALUE_ERROR);
            }
            if ($this->varName === 'nil') {
                return $this->typeConverter->toObject(null);
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

    private function evaluateLiteral(): mixed
    {
        switch ($this->literal->className) {
            case 'Integer':
                if (!is_numeric($this->literal->value)) {
                    $this->typeConverter->program->stderr->writeString("Invalid integer");
                    exit(ReturnCode::INTERPRET_VALUE_ERROR);
                }
                return (int)$this->literal->value;
            case 'String':
                // handling escape seqs
                $value = $this->literal->value;
                $result = '';
                $i = 0;
                $len = strlen($value);

                while ($i < $len) {
                    if ($value[$i] === '\\') {
                        switch ($value[$i + 1]) {
                            case 'n':
                                $result .= "\n";
                                break;
                            case '\\':
                                $result .= "\\";
                                break;
                            case '\'':
                                $result .= "'";
                                break;
                            default:
                                $result .= $value[$i + 1];
                        }
                        $i += 2;
                    } else {
                        $result .= $value[$i];
                        $i++;
                    }
                }

                return $result;
            case 'True':
                return $this->typeConverter->toObject(true);
            case 'False':
                return $this->typeConverter->toObject(false);
            case 'Nil':
                return $this->typeConverter->toObject(null);
            case 'class':
                return $this->literal->value;
            default:
                $this->typeConverter->program->stderr->writeString("Unknown type");
                exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
    }

    private function evaluateSend(Execution $context): mixed
    {
        $args = $this->evaluateArguments($context);

        if ($this->selector === 'new' && isset($args[0]) && is_string($args[0])) {
            $class = $context->self->program->getClass($args[0]);
            if (!$class) {
                $context->self->program->stderr->writeString("Class not found");
                exit(ReturnCode::INTERPRET_TYPE_ERROR);
            }

            $classObj = new ObjectI($class, $context->self->program);
            return $classObj->sendMessage('new', array_slice($args, 1));
        }

        if ($this->selector === 'from:' && is_string($args[0])) {
            return $this->createClassInstance($args[0], array_slice($args, 1), $context);
        }

        if (empty($args)) {
            return $this->typeConverter->toObject(null);
        }

        try {
            $receiver = $this->typeConverter->toObject($args[0]);
            return $receiver->sendMessage($this->selector, array_slice($args, 1));
        } catch (\Throwable $e) {
            $context->self->program->stderr->writeString("Failed to convert");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
    }

    /**
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
            $context->self->program->stderr->writeString("Class not found");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }

        $value = $args[0] ?? null;
        if ($className === 'Integer' && $value !== null) {
            if (!is_numeric($value)) {
                $context->self->program->stderr->writeString("Cannot convert");
                exit(ReturnCode::INTERPRET_VALUE_ERROR);
            }
            $value = (int)$value;
        }

        $instance = new ObjectI($class, $context->self->program);
        $instance->attributes['value'] = $value;
        return $instance;
    }

    // binds the block to the outer context
    private function evaluateBlock(Execution $outer): callable
    {
        if (empty($this->blocks)) {
            $outer->self->program->stderr->writeString("No block");
            exit(ReturnCode::INTERPRET_VALUE_ERROR);
        }

        $block = $this->blocks[0];
        return $block->bindContext($outer);
    }
}
