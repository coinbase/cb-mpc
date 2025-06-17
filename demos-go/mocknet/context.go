package mocknet

import (
	"container/list"
	"errors"
	"sync"
)

type DemoDataTransport struct {
	roleIndex int
	outs      []*DemoDataTransport
	mutex     sync.Mutex
	cond      *sync.Cond
	queues    []list.List
	isAbort   bool
}

func NewDemoDataTransport(roleIndex int) *DemoDataTransport {
	ctx := &DemoDataTransport{roleIndex: roleIndex}
	ctx.cond = sync.NewCond(&ctx.mutex)
	ctx.isAbort = false
	return ctx
}

func (dt *DemoDataTransport) setOuts(dts []*DemoDataTransport) {
	dt.outs = dts
	dt.queues = make([]list.List, len(dts))
}

func (dt *DemoDataTransport) MessageSend(receiverIndex int, buffer []byte) error {
	if receiverIndex == dt.roleIndex {
		return errors.New("cannot send to self")
	}

	receiverDT := dt.outs[receiverIndex]
	receiverDT.mutex.Lock()
	receiverDT.queues[dt.roleIndex].PushBack(buffer)
	receiverDT.mutex.Unlock()
	receiverDT.cond.Broadcast()

	return nil
}

func (dt *DemoDataTransport) MessageReceive(senderIndex int) ([]byte, error) {
	if senderIndex == dt.roleIndex {
		return nil, errors.New("cannot receive from self")
	}

	dt.mutex.Lock()
	defer dt.mutex.Unlock()

	if dt.isAbort {
		return nil, errors.New("aborted")
	}
	queue := &dt.queues[senderIndex]
	for queue.Len() == 0 {
		dt.cond.Wait()
		if dt.isAbort {
			return nil, errors.New("aborted")
		}
	}
	front := queue.Front()
	receivedMsg := front.Value.([]byte)
	queue.Remove(front)
	return receivedMsg, nil
}

func (dt *DemoDataTransport) MessagesReceive(senderIndices []int) (receivedMsgs [][]byte, err error) {
	n := len(senderIndices)
	receivedMsgs = make([][]byte, n)

	var wg sync.WaitGroup
	wg.Add(n)
	for i, senderIndex := range senderIndices {
		go func(i int, senderIndex int) {
			var e error
			receivedMsgs[i], e = dt.MessageReceive(senderIndex)
			if e != nil {
				err = e // atomic ?
			}
			wg.Done()
		}(i, senderIndex)
	}
	wg.Wait()

	return receivedMsgs, nil
}
