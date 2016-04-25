<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */


/**
 * @Entity @Table(name="entities")
 **/
class Entity
{
    /**
     * @var int
     * @Id @Column(type="integer") @GeneratedValue
     */
    protected $id;
    /**
     * @Column(type="datetime",nullable=false)
     */
    protected $timestamp;
    /**
     * @Column(type="decimal",nullable=false)
     */
    protected $value;
    /**
     * @ManyToOne(targetEntity="Station")
     */
    protected $station;
    /**
     * @ManyToOne(targetEntity="Experiment")
     */
    protected $experiment;


    public function getId() {
        return $this->id;
    }

    public function getValue() {
        return $this->value;
    }

    public function setValue($d) {
        $this->value = $d;
    }

    public function getTimestamp() {
        return $this->timestamp;
    }

    public function setTimestamp($s) {
        $this->timestamp = $s;
    }

    public function getStation() {
        return $this->station;
    }

    public function setStation($m) {
        $this->stations = $m;
    }

    public function getExperiment() {
        return $this->experiment;
    }

    public function setExperiment($f) {
        $this->experiment = $f;
    }
}

?>
