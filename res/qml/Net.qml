import QtQuick 2.14
import QtQuick.Controls 2.4
import "../js/VisuFunction.js" as VisuFunction
NetData {
    id:netItem
    width: 600
    height: 600
    property int visuNeuronModus: 0
    property bool enableUpdateTimer: visuNeuronModus===def.functionVisu? false:true
    property int visu: 0
    property bool enableMousArea: false
    property int clickedNeuronID: -1
    property int clickedNeuronType: 0
    property bool moveable: false
    property int maxYNeuron: {
        var b
        if(bias===true)b=1
        else b=0
        if(hiddenNeuronY>=outputNeuron&&hiddenNeuronY>=inputNeuron){
            return hiddenNeuronY+b
        }
        else if(hiddenNeuronY<=outputNeuron&&outputNeuron>=inputNeuron){
            return outputNeuron+b
        }
        else if(hiddenNeuronY<=inputNeuron&&outputNeuron<=inputNeuron){
            return inputNeuron+b
        }
        return 0
    }
    property real dRelationship: 0.8
    property int xDistance: {
        var w
        if(((width)-xOffSet*2)/(hiddenNeuronX+1)*dRelationship<((height)/(maxYNeuron-1+yOffSet*2))*dRelationship){
            w= ((width)-xOffSet*2)/(hiddenNeuronX+1)*dRelationship}
        else w= ((height)/(maxYNeuron-1+yOffSet*2))*dRelationship
        return (width-w)/(hiddenNeuronX+1+yOffSet*2)
    }
    property int yDistance: {
        var w
        if(((width)-xOffSet*2)/(hiddenNeuronX+1)*dRelationship<((height)/(maxYNeuron-1+yOffSet*2))*dRelationship){
            w= ((width)-xOffSet*2)/(hiddenNeuronX+1)*dRelationship}
        else w= ((height)/(maxYNeuron-1+yOffSet*2))*dRelationship
        return (height-w)/(maxYNeuron-1+yOffSet*2)
    }
    property int d:{
        var dP
        if(yDistance<xDistance) dP =yDistance*dRelationship
        else dP= xDistance*dRelationship
        return dP
    }
    property variant hiddenConXInput: []
    property variant hiddenConYInput: []

    property variant hiddenConXOutput: []
    property variant hiddenConYOutput: []

    property variant outputConXInput: []
    property variant outputConYInput: []

    property variant inputConXOutput: []
    property variant inputConYOutput: []

    property variant biasConXOutput: []
    property variant biasConYOutput: []

    property variant hiddenTransparent: new Array(totalHidden).fill(100)
    property variant biasTransparent: new Array(hiddenNeuronX+1).fill(100)
    property variant inputTransparent: new Array(inputNeuron).fill(100)
    property variant outputTransparent: new Array(outputNeuron).fill(100)
    property variant conTransparent: new Array(conSourceID.length).fill(100)
    property bool neuronClickEnable: true
    property real yOffSet: 0.1
    property real xOffSet: 0.1
    property int yBiasPos: if(bias) return 1
                        else return 0

    property bool showId: true
    signal clickedNet(var id)
    DropArea {
        anchors.fill: parent
    }

    MouseArea{
        anchors.fill: parent
        enabled: false//enableMousArea
        onClicked: {
            console.debug("click")
            clickedNet(netID)}
    }
    MouseArea{
        anchors.fill: parent
        onClicked: setTransparancy(100)
    }
    Repeater{
        model: conSourceID.length
        NeuronConnection{
            conID: index
            weight:conWeight[index]
            transparent:conTransparent[index]
            d:netItem.d
        }
    }
    Repeater{
        id:biasLayer
        model: hiddenNeuronX+1
        visible: bias
        Neuron{
            xRel:(xOffSet+index)//*xDistance
            yRel: yOffSet//t*yDistance
            d:netItem.d
            neuronValue: biasValue
            typeId: index
            type:def.biasType
            lastNeuron: (index==(biasLayer.model-1))
            onClickedNeuron: {
                clickedNeuronID= typeId
                clickedNeuronType= type
            }
            transparent: biasTransparent[index]

        }
    }

    Repeater{
        id:inputLayer
        model:inputNeuron
        Neuron{
            xRel:xOffSet//*xDistance
            yRel:index+yOffSet+yBiasPos//*yDistance+yBiasPos
            d:netItem.d

            neuronValue: if(inputValue.length>typeId) return inputValue[typeId]
                            else return 0
            typeId: index
            type:def.inputType
            neuronID:index
            lastNeuron: (index==(inputLayer.model-1))
            onClickedNeuron: {
                clickedNeuronID= typeId
                clickedNeuronType= type
            }
            transparent: inputTransparent[index]

        }
    }

    Repeater{
        id:hiddenXLayer
        model:hiddenNeuronX
        Repeater{
            id:hiddenYLayer
            model:hiddenNeuronY
            property int indexX: index
            Neuron{
                xRel:(indexX+xOffSet+1)//*xDistance
                yRel:index+yOffSet+yBiasPos//*yDistance
                d:netItem.d
                typeId: index+(indexX*hiddenNeuronY)
                neuronID:if(0<hiddenIDs[typeId]){
                             return hiddenIDs[typeId]}
                            else return typeId
                neuronValue: if(index+(indexX*hiddenNeuronY)<hiddenValue.length){
                                         return hiddenValue[typeId]}
                             else return 0
                type:def.hiddenType
                lastNeuron: (indexX==(hiddenXLayer.model-1)&&index===(hiddenYLayer.model-1))
                onClickedNeuron: {
                    clickedNeuronID= typeId
                    clickedNeuronType= type
                }
                transparent: hiddenTransparent[typeId]
        }
    }
    }
    Repeater{
        id:outputLayer
        model:outputNeuron
        Neuron{
            xRel:(xOffSet+1+hiddenNeuronX)//xDistance
            yRel:(index+yOffSet)//yDistance+yBiasPos
            d:netItem.d
            typeId: index
            neuronID: if(outputIds.length>typeId) return outputIds[typeId]
                        else return typeId+totalHidden
            type:def.outputType
            neuronValue: if(outputValue.length>typeId) outputValue[index]
                            else return 0
            lastNeuron: (index==(outputLayer.model-1))
            onClickedNeuron: {
                clickedNeuronID= typeId
                clickedNeuronType= type
            }
            transparent: outputTransparent[index]

        }
    }


    Text {
        anchors.top:parent.top
        anchors.right: parent.right
        anchors.rightMargin: netItem.width*xOffSet

        anchors.topMargin: yOffSet
        topPadding: yOffSet
        text: "net Id: "+netID
        font.pixelSize: yOffSet

        visible:showId
        horizontalAlignment: Text.AlignRight
    }
    function setTransparancy(trans){
        hiddenTransparent= new Array(totalHidden).fill(trans)
        biasTransparent= new Array(hiddenNeuronX+1).fill(trans)
        inputTransparent= new Array(inputNeuron).fill(trans)
        outputTransparent= new Array(outputNeuron).fill(trans)
        conTransparent= new Array(conSourceID.length).fill(trans)

    }
    function updateTransparancy(){
        hiddenTransparent=VisuFunction.updateArray(hiddenTransparent)
        outputTransparent=VisuFunction.updateArray(outputTransparent)
        inputTransparent=VisuFunction.updateArray(inputTransparent)
        biasTransparent=VisuFunction.updateArray(biasTransparent)
        conTransparent=VisuFunction.updateArray(conTransparent)
    }

    function setHighlight(id,type,highlightValue){
        if(type===def.hiddenType){
            hiddenTransparent[id]=highlightValue
        }
        else if(type===def.outputType){
            outputTransparent[id-totalHidden]=highlightValue
        }
        else if(type===def.inputType){
            inputTransparent[id]=highlightValue
        }
        else if(type===def.biasType){
            biasTransparent[id]=highlightValue
        }
    }

    function setNetHighlight(id,type){
        var highlightValue=100
        setTransparancy(20)
        if(type===def.outputType) id+=totalHidden
        setHighlight(id,type,highlightValue)
        if(type===def.outputType) id-=totalHidden
        var arrConId=[]
        var sType
        var sId
        arrConId=getConSource(id,type)
        for(var i=0;i<arrConId.length;i++){

            sId=conDestinationID[arrConId[i]]
            sType=conDestinationType[arrConId[i]]
            conTransparent[arrConId[i]]=highlightValue
            setHighlight(sId,sType,highlightValue)
        }
        arrConId=getConDestination(id,type)
        for(i=0;i<arrConId.length;i++){
            sId=conSourceID[arrConId[i]]
            sType=conSourceType[arrConId[i]]
            if(sType===def.biasType){
                if(type===def.outputType)sId=hiddenNeuronX
                else sId=Math.floor(id/hiddenNeuronY)
            }
            conTransparent[arrConId[i]]=highlightValue
            setHighlight(sId,sType,highlightValue)
        }
        updateTransparancy()
    }
}
