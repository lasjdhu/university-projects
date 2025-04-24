<?php

/**
 * IPP - Program structure
 * @author Dmitrii Ivanushkin (xivanu00)
 */

namespace IPP\Student;

use IPP\Student\Primitives\ClassPrimitive;
use IPP\Student\Environment\ObjectI;
use IPP\Core\Interface\OutputWriter;
use IPP\Core\Interface\InputReader;
use IPP\Core\ReturnCode;
use IPP\Student\Builtins\{ObjectClass, IntegerClass, StringClass, BooleanClass, NilClass, BlockClass};

class Program
{
    private static ?Program $instance = null;

    /** @var ClassPrimitive[] */
    public $classes = [];
    /** @var array<string, ClassPrimitive> */
    private array $classMap = [];
    public OutputWriter $stdout;
    public InputReader $stdin;
    public OutputWriter $stderr;

    public function __construct(OutputWriter $stdout, InputReader $stdin, OutputWriter $stderr)
    {
        $this->stdout = $stdout;
        $this->stdin = $stdin;
        $this->stderr = $stderr;
        self::$instance = $this;
    }

    public static function getInstance(): ?Program
    {
        return self::$instance;
    }

    public function getClass(string $name): ?ClassPrimitive
    {
        return $this->classMap[$name] ?? null;
    }

    public function execute(): void
    {
        $this->registerBuiltinClasses();

        foreach ($this->classes as $class) {
            $this->classMap[$class->name] = $class;
        }

        $mainClass = $this->getClass("Main");
        if (!$mainClass) {
            $this->stderr->writeString("Main class not found");
            exit(ReturnCode::PARSE_MAIN_ERROR);
        }

        // program execution starts by sending 'run' message to Main class instance
        $mainInstance = new ObjectI($mainClass, $this);
        $mainInstance->sendMessage("run", []);
    }

    private function registerBuiltinClasses(): void
    {
        $object = new ClassPrimitive("Object", null);
        ObjectClass::addMethods($object);
        $this->classMap["Object"] = $object;

        $integer = new ClassPrimitive("Integer", "Object");
        IntegerClass::addMethods($integer);
        $this->classMap["Integer"] = $integer;

        $string = new ClassPrimitive("String", "Object");
        StringClass::addMethods($string, $this->stdout, $this->stdin);
        $this->classMap["String"] = $string;

        $true = new ClassPrimitive("True", "Object");
        BooleanClass::addMethods($true);
        $this->classMap["True"] = $true;

        $false = new ClassPrimitive("False", "Object");
        BooleanClass::addMethods($false);
        $this->classMap["False"] = $false;

        $nil = new ClassPrimitive("Nil", "Object");
        NilClass::addMethods($nil);
        $this->classMap["Nil"] = $nil;

        $block = new ClassPrimitive("Block", "Object");
        BlockClass::addMethods($block);
        $this->classMap["Block"] = $block;
    }
}
