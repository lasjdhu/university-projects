<?php

/**
 * IPP - XML Parser
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student;

use DOMElement;
use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Primitives\MethodPrimitive;
use IPP\Student\Primitives\BlockPrimitive;
use IPP\Student\Primitives\ExpressionPrimitive;
use IPP\Student\Primitives\StatementPrimitive;
use IPP\Student\Primitives\LiteralPrimitive;

class Parser
{
    private DOMElement $root;

    public function __construct(\DOMDocument $dom)
    {
        $this->root = $dom->documentElement;
    }

    public function parse(Program $program): void
    {
        foreach ($this->root->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'class') {
                $parsedClass = $this->parseClass($node);
                $program->classes[] = $parsedClass;
            }
        }
    }

    private function parseClass(DOMElement $classNode): ClassPrimitive
    {
        $className = $classNode->getAttribute('name');
        $parentName = $classNode->getAttribute('parent');
        $classPrimitive = new ClassPrimitive($className, $parentName);

        foreach ($classNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'method') {
                $parsedMethod = $this->parseMethod($node);
                $classPrimitive->methods[] = $parsedMethod;
            }
        }

        return $classPrimitive;
    }

    private function parseMethod(DOMElement $methodNode): MethodPrimitive
    {
        $selector = $methodNode->getAttribute('selector');
        $methodPrimitive = new MethodPrimitive($selector);

        foreach ($methodNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'block') {
                $parsedBlock = $this->parseBlock($node);
                $methodPrimitive->blocks[] = $parsedBlock;
            }
        }

        return $methodPrimitive;
    }

    private function parseBlock(DOMElement $blockNode): BlockPrimitive
    {
        $arity = (int)$blockNode->getAttribute('arity');
        $blockPrimitive = new BlockPrimitive($arity);

        // parameters must be sorted by order before assigning to ensure correct argument binding
        $parameters = [];
        foreach ($blockNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'parameter') {
                $order = (int)$node->getAttribute('order');
                $name = $node->getAttribute('name');
                $parameters[$order] = $name;
            }
        }

        ksort($parameters);
        $blockPrimitive->setParameters(array_values($parameters));

        foreach ($blockNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'assign') {
                $parsedStatement = $this->parseStatement($node);
                $blockPrimitive->statements[] = $parsedStatement;
            }
        }

        return $blockPrimitive;
    }

    private function parseStatement(DOMElement $statementNode): StatementPrimitive
    {
        $order = (int)$statementNode->getAttribute('order');
        $statementPrimitive = new StatementPrimitive($order);

        foreach ($statementNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'var') {
                $statementPrimitive->varName = $node->getAttribute('name');
            } elseif ($node instanceof DOMElement && $node->tagName === 'expr') {
                $parsedExpr = $this->parseExpression($node);
                $statementPrimitive->expressions[] = $parsedExpr;
            }
        }

        return $statementPrimitive;
    }

    private function parseExpression(DOMElement $exprNode): ExpressionPrimitive
    {
        $exprPrimitive = new ExpressionPrimitive();

        foreach ($exprNode->childNodes as $node) {
            if (!($node instanceof DOMElement)) {
                continue;
            }

            switch ($node->tagName) {
                case 'block':
                    $parsedBlock = $this->parseBlock($node);
                    $exprPrimitive->blocks[] = $parsedBlock;
                    break;

                case 'send':
                    $this->parseSend($node, $exprPrimitive);
                    break;

                case 'literal':
                    $className = $node->getAttribute('class');
                    $value = $node->getAttribute('value');
                    $exprPrimitive->literal = new LiteralPrimitive($className, $value);
                    break;

                case 'var':
                    $exprPrimitive->varName = $node->getAttribute('name');
                    break;
            }
        }

        return $exprPrimitive;
    }

    private function parseSend(DOMElement $sendNode, ExpressionPrimitive $exprPrimitive): void
    {
        $exprPrimitive->selector = $sendNode->getAttribute('selector');

        // message send parsing handles both the receiver (expr) and optional arguments
        foreach ($sendNode->childNodes as $node) {
            if ($node instanceof DOMElement && $node->tagName === 'arg') {
                $order = (int)$node->getAttribute('order');
                foreach ($node->childNodes as $childNode) {
                    if ($childNode instanceof \DOMElement && $childNode->tagName === 'expr') {
                        $argExpr = $this->parseExpression($childNode);
                        $argExpr->argOrder = $order;
                        $exprPrimitive->arguments[] = $argExpr;
                    }
                }
            } elseif ($node instanceof DOMElement && $node->tagName === 'expr') {
                // the receiver expression (target of the message) has order 0
                $argExpr = $this->parseExpression($node);
                $argExpr->argOrder = 0;
                $exprPrimitive->arguments[] = $argExpr;
            }
        }
    }
}
