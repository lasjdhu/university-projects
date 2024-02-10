<?php

/**
 * @brief Line (STDIN) class module
 * @file Line.php
 * @author Dmitrii Ivanushkin xivanu00
 */

class Line {
    
    public $content;

    /**
     * @brief Get line by line from STDIN and stripe whitespaces within
     */
    public function __construct() {
        if (!STDIN)
            exit(INPUT_ERR);
        $this->content = trim(fgets(STDIN));
    }

    /**
     * @brief Check every char for "#" character and delete everything from its position till EOL
     */
    public function deleteComments() {
        $chars = str_split($this->content);

        foreach ($chars as $i => $char) {
            if ($char == "#")
                $this->content = trim(substr($this->content, 0, $i));
        }
    }
    
}

?>