<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */


/**
 * @Entity @Table(name="types")
 **/
class Type
{
    /**
     * @var int
     * @Id @Column(type="integer") @GeneratedValue
     */
    protected $id;

    /**
     * @var string
     * @Column(type="string")
     */
    protected $description;


    public function getId() {
        return $this->id;
    }

    public function getDescription() {
        return $this->description;
    }
    public function setDescription($d) {
        $this->description = $d;
    }

}
?>
