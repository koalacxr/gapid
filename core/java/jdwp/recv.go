// Copyright (C) 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package jdwp

import (
	"bytes"
	"io"
	"reflect"

	"github.com/google/gapid/core/context/jot"
	"github.com/google/gapid/core/data/endian"
	"github.com/google/gapid/core/event/task"
	"github.com/google/gapid/core/log"
	"github.com/google/gapid/core/os/device"
)

// recv decodes all the incoming reply or command packets, forwarding them on
// to the corresponding chans. recv is blocking and should be run on a new
// go routine.
// recv returns when ctx is stopped or there's an IO error.
func (c *Connection) recv(ctx log.Context) {
	for !task.Stopped(ctx) {
		packet, err := c.readPacket()
		switch err {
		case nil:
		case io.EOF:
			return
		default:
			if !task.Stopped(ctx) {
				jot.Warning(ctx).Cause(err).Print("Failed to read packet")
			}
			return
		}

		switch packet := packet.(type) {
		case replyPacket:
			c.Lock()
			out, ok := c.replies[packet.id]
			c.Unlock()
			if !ok {
				jot.Warning(ctx).With("id", packet.id).Cause(err).Print("Unexpected reply")
				continue
			}
			out <- packet

		case cmdPacket:
			switch {
			case packet.cmdSet == cmdSetEvent && packet.cmdID == cmdCompositeEvent:
				d := endian.Reader(bytes.NewReader(packet.data), device.BigEndian)
				events := Events{}
				if err := c.decode(d, reflect.ValueOf(&events)); err != nil {
					jot.Fail(ctx, err, "Couldn't decode composite event data")
					continue
				}
				c.Lock()
				for _, handler := range c.events {
					handler <- events
				}
				c.Unlock()

			default:
				// Unknown packet. Ignore.
			}
		}
	}
}
