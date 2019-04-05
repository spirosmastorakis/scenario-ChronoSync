/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Spyridon (Spyros) Mastorakis <mastorakis@cs.ucla.edu>
 */

#include "chronosync.hpp"

namespace ndn {

ChronoSync::ChronoSync(const int minNumberMessages, const int maxNumberMessages)
  : m_face(m_ioService)
  , m_scheduler(m_ioService)
  , m_randomGenerator(static_cast<unsigned int>(std::time(0)))
  , m_rangeUniformRandom(m_randomGenerator, boost::uniform_int<>(1000, 3000))
  , m_messagesUniformRandom(m_randomGenerator, boost::uniform_int<>(minNumberMessages, maxNumberMessages))
  , m_numberMessages(m_messagesUniformRandom())
{
}

void
ChronoSync::setSyncPrefix(const Name& syncPrefix)
{
  m_syncPrefix = syncPrefix;
}

void
ChronoSync::setUserPrefix(const Name& userPrefix)
{
  m_userPrefix = userPrefix;
}

void
ChronoSync::setRoutingPrefix(const Name& routingPrefix)
{
  m_routingPrefix = routingPrefix;
}

void
ChronoSync::delayedInterest(int id)
{
  std::cout << "Delayed Interest with id: " << id << "\n";
  m_socket->publishData(reinterpret_cast<const uint8_t*>(std::to_string(id).c_str()),
                        std::to_string(id).size(), ndn::time::milliseconds(4000));


  if (id < m_numberMessages)
    m_scheduler.scheduleEvent(ndn::time::milliseconds(m_rangeUniformRandom()),
                              bind(&ChronoSync::delayedInterest, this, ++id));
}

void
ChronoSync::publishDataPeriodically(int id)
{
  m_scheduler.scheduleEvent(ndn::time::milliseconds(m_rangeUniformRandom()),
                            bind(&ChronoSync::delayedInterest, this, 1));

  m_scheduler.scheduleEvent(ndn::time::milliseconds(m_rangeUniformRandom()),
                            bind(&ChronoSync::publishDataPeriodically, this, 1));
}


void
ChronoSync::printData(const Data& data)
{
  Name::Component peerName = data.getName().at(1);

  std::string s (reinterpret_cast<const char*>(data.getContent().value()),
                 data.getContent().value_size());

  std::cout << "Data received from " << peerName.toUri() << " : " <<  s << "\n";
}


void
ChronoSync::processSyncUpdate(const std::vector<chronosync::MissingDataInfo>& updates)
{
  std::cout << "Process Sync Update \n";
  if (updates.empty()) {
    return;
  }

  for (unsigned int i = 0; i < updates.size(); i++) {
    for (chronosync::SeqNo seq = updates[i].low; seq <= updates[i].high; ++seq) {
      m_socket->fetchData(updates[i].session, seq,
                          bind(&ChronoSync::printData, this, _1),
                          2);
    }

  }
}

void
ChronoSync::initializeSync()
{
  std::cout << "ChronoSync Instance Initialized \n";
  m_routableUserPrefix = Name();
  m_routableUserPrefix.clear();
  m_routableUserPrefix.append(m_routingPrefix).append(m_userPrefix);

  m_socket = std::make_shared<chronosync::Socket>(m_syncPrefix,
                                                  m_routableUserPrefix,
                                                  m_face,
                                                  bind(&ChronoSync::processSyncUpdate, this, _1));

}

void
ChronoSync::run()
{
  m_scheduler.scheduleEvent(ndn::time::milliseconds(m_rangeUniformRandom()),
                            bind(&ChronoSync::delayedInterest, this, 1));
}

void
ChronoSync::runPeriodically()
{
  m_scheduler.scheduleEvent(ndn::time::milliseconds(m_rangeUniformRandom()),
                            bind(&ChronoSync::publishDataPeriodically, this, 1));
}

} // namespace ndn
