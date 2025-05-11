<?php

/**
 * IPP - Execution structure
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student\Environment;

use IPP\Core\ReturnCode;

class Execution
{
    public ObjectI $self;
    /** @var mixed[] */
    public array $arguments;
    /** @var string[] */
    public array $parameters;
    /** @var mixed[] */
    private array $variables = [];

    /**
     * @param ObjectI $self
     * @param mixed[] $arguments
     * @param string[] $parameters
     */
    public function __construct(ObjectI $self, array $arguments, array $parameters)
    {
        $this->self = $self;
        $this->arguments = $arguments;
        $this->parameters = $parameters;

        // bind parameters to arguments and define special variables
        foreach ($parameters as $i => $name) {
            $this->variables[$name] = $arguments[$i] ?? null;
        }
        $converter = new TypeConverter($self->program);
        $this->variables['self'] = $self;
        $this->variables['true'] = $converter->toObject(true);
        $this->variables['false'] = $converter->toObject(false);
        $this->variables['nil'] = $converter->toObject(null);
    }

    public function getVariable(string $name): mixed
    {
        if (!isset($this->variables[$name])) {
            $this->self->program->stderr->writeString("Variable not found");
            exit(ReturnCode::INTERPRET_TYPE_ERROR);
        }
        return $this->variables[$name];
    }

    /**
     * @param string $name
     * @param mixed $value
     */
    public function setVariable(string $name, $value): void
    {
        $this->variables[$name] = $value;
    }
}
