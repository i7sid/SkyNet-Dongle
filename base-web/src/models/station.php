<?php
/**
 * @author Michael Zapf <michael.zapf@fau.de>
 * @version 0.1
 * @copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */


/**
 * @Entity @Table(name="stations")
 **/
class Station
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
    protected $longitude;
    /**
     * @var string
     * @Column(type="string")
     */
    protected $latitude;
    /**
     * @var string
     * @Column(type="string")
     */
    protected $mac;


    public function getId() {
        return $this->id;
    }

    public function getLongitude() {
        return $this->longitude;
    }

    public function setLongitude($d) {
        $this->longitude = $d;
    }

    public function getLatitude() {
        return $this->latitude;
    }

    public function setLatitude($d) {
        $this->latitude = $d;
    }

    public function getMac() {
        return $this->mac;
    }

    public function setMac($m) {
        $this->mac = $d;
    }

}

?>
