<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */


/**
 * @Entity @Table(name="experiments")
 **/
class Experiment
{
    /**
     * @var int
     * @Id @Column(type="integer") @GeneratedValue
     */
    protected $id;


    public function getId() {
        return $this->id;
    }

}
?>
