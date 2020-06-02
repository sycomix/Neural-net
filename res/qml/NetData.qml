import QtQuick 2.0
import "../js/VisuFunction.js" as VisuFunction
Item {
    property int netID: 0
    property int updateTime:100

    property bool enableUpdateTimer: true
    property bool forceTimer: false

    property int hiddenNeuronX: netListVisu.getHiddenX(netID)
    property int hiddenNeuronY: netListVisu.getHiddenY(netID)
    property int totalHidden: hiddenNeuronY*hiddenNeuronX
    property int inputNeuron: netListVisu.getInputs(netID)
    property int outputNeuron: netListVisu.getOutputs(netID)
    property variant hiddenValue: netListVisu.getHiddenValue(netID)
    property variant outputValue:netListVisu.getOutputsValue(netID)
    property variant inputValue:netListVisu.getInputsValue(netID)
    property int clickedNeuronID: -1
    property int clickedNeuronType: 0

    property bool bias: netListVisu.getBias(netID)
    property real biasValue: netListVisu.getBiasValue(netID)

    property variant conWeight: netListVisu.getConWeight(netID)

    property variant hiddenIDs: netListVisu.getHiddenID(netID)
    property variant outputIds: netListVisu.getOutputID(netID)

    property variant conSourceID:netListVisu.getConSourceID(netID)
    property variant conDestinationID: netListVisu.getConDestinationID(netID)

    property variant conSourceType: netListVisu.getConSourceType(netID)
    property variant conDestinationType: netListVisu.getConDestinationType(netID)

    property int noneType: 0
    property int inputType: 1
    property int hiddenType: 2
    property int outputType: 3
    property int biasType: 5
    Connections {
                   target: netListVisu
                   onStopUpdateSignal: timerNet.running=false
                   onStartUpdateSignal:{timerNet.running=true
                       updateStructur()}
                   onSetUpdateTimeSignal:if(!forceTimer) updateTime=time

    }
    Timer {
        id:timerNet
            interval: updateTime; running: enableUpdateTimer
             repeat: true
            onTriggered: {netListVisu.displayUpdatNetTimer(netID)
                updateValue()
            }
        }
    function updateValue(){
        hiddenValue=netListVisu.getHiddenValue(netID)
        inputValue=netListVisu.getInputsValue(netID)
        outputValue=netListVisu.getOutputsValue(netID)
        biasValue=netListVisu.getBiasValue(netID)
        conWeight=netListVisu.getConWeight(netID)
        /*
        */
    }
    function updateStructur(){
        //console.debug("update Struc")
        inputNeuron=netListVisu.getInputs(netID)
        outputNeuron=netListVisu.getOutputs(netID)
        hiddenNeuronX=netListVisu.getHiddenX(netID)
        hiddenNeuronY=netListVisu.getHiddenY(netID)

        hiddenIDs=netListVisu.getHiddenID(netID)
        outputIds=netListVisu.getOutputID(netID)
        conSourceID=netListVisu.getConSourceID(netID)
        conSourceType=netListVisu.getConSourceType(netID)
        conDestinationID=netListVisu.getConDestinationID(netID)
        conDestinationType=netListVisu.getConDestinationType(netID)

        bias=netListVisu.getBias(netID)
    }
    function getTypeString(type){
        if(type===noneType){
            return "noneType"}
        else if(type===inputType){
            return "inputType"}
        else if(type===hiddenType){
            return "hiddenType"}
        else if(type===outputType){
            return "outputType"}
        else if(type===biasType){
            return "biasType"}
        else return ""
    }
    function getConSource(id,type){

        return chekArrayForValues(conSourceID,conSourceType,id,type,conDestinationID)
    }
    function getConDestination(id,type){
        return chekArrayForValues(conDestinationID,conDestinationType,id,type,conSourceID)
    }

    function chekArrayForValues(idArr,typeArr,id,type,conIdArr){
        var orgId
        orgId=id
        if(type===outputType){
            id +=totalHidden
        }
        else if(type===biasType){
            id=0
        }
        var res=[]
        //console.debug(arr1,arr2,val1,val2)
        for(var i=0;i<idArr.length;i++){
            if(idArr[i]===id&&typeArr[i]===type){
                if(type!==biasType||Math.floor(conIdArr[i]/hiddenNeuronY)===orgId){
                    res.push(i)}
            }

        }
        return res
    }
}