#pragma once
#include <chrono>
#include <map>
#include <random>
#include <thread>
#include <zmq.hpp>

#include "Message.hpp"
#include "../Printer.hpp"
#include "NodeId.hpp"

namespace NodeSystem {
    class SensorManager {
    private:
        struct VirtualSensor {
            NodeId sensorId;
            NodeId targetNodeId;
            std::chrono::steady_clock::time_point next_fire_time;
            zmq::socket_t *socket; // Вказівник на сокет для цільової системи
        };

        zmq::context_t *m_context; // Спільний контекст
        std::thread m_thread;
        std::atomic<bool> m_running;

        std::vector<VirtualSensor> m_sensors;
        std::mutex m_sensors_mutex;

        // Один сокет-відправник на кожну унікальну адресу системи
        std::map<std::string, std::unique_ptr<zmq::socket_t> > m_senders;

        // Генератори випадкових чисел
        std::mt19937 m_rng;
        std::uniform_real_distribution<> m_val_dist;
        std::uniform_int_distribution<> m_time_dist_ms;

        // Допоміжна функція: отримує або створює сокет для адреси
        zmq::socket_t *getSocketForAddress(const std::string &addr) {
            if (m_senders.find(addr) == m_senders.end()) {
                Printer::print_safe("[SensorManager] Створення сокету для " + addr);
                auto new_socket = std::make_unique<zmq::socket_t>(*m_context, zmq::socket_type::push);
                new_socket->connect(addr.c_str());
                m_senders[addr] = std::move(new_socket);
            }
            return m_senders[addr].get();
        }

        void run() {
            while (m_running) {
                auto now = std::chrono::steady_clock::now();

                std::lock_guard lock(m_sensors_mutex);
                for (auto &sensor: m_sensors) {
                    if (now >= sensor.next_fire_time) {
                        // 1. Час спрацювати
                        double value = m_val_dist(m_rng);
                        MessageReceive task{sensor.sensorId, sensor.targetNodeId, value};

                        zmq::message_t zmq_msg(sizeof(MessageReceive));
                        memcpy(zmq_msg.data(), &task, sizeof(MessageReceive));

                        try {
                            sensor.socket->send(zmq_msg, zmq::send_flags::none);
                        } catch (const zmq::error_t &e) {
                            Printer::print_safe("[SensorManager] Помилка ZMQ: " + std::string(e.what()));
                        }

                        // 2. Перепланувати
                        int sleep_ms = m_time_dist_ms(m_rng);
                        sensor.next_fire_time = now + std::chrono::milliseconds(sleep_ms);
                    }
                }

                // Спимо, щоб не навантажувати CPU
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

    public:
        SensorManager(zmq::context_t *context)
            : m_context(context),
              m_running(false),
              m_rng(std::random_device{}()),
              m_val_dist(-1.0, 1.0),
              m_time_dist_ms(500, 10000) // 0.5 - 10 секунд
        {
        }

        ~SensorManager() {
            stop();
        }

        // Це і є "прив'язка" сенсора
        void addSensor(int sensorId, const std::string &targetSystemTaskAddr, NodeId targetNodeId) {
            std::lock_guard lock(m_sensors_mutex);

            zmq::socket_t *socket = getSocketForAddress(targetSystemTaskAddr);
            NodeId id = {998, sensorId}; // Рівень 998 для сенсорів

            // Початковий час спрацьовування - випадковий, щоб вони не запускалися всі разом
            int initial_delay = m_time_dist_ms(m_rng) / 2;
            auto next_fire = std::chrono::steady_clock::now() + std::chrono::milliseconds(initial_delay);

            m_sensors.push_back({id, targetNodeId, next_fire, socket});
            Printer::print_safe(
                "[SensorManager] Додано сенсор " + id.to_string() + " -> " + targetNodeId.to_string() + " @ " +
                targetSystemTaskAddr);
        }

        void start() {
            if (m_running) return;
            m_running = true;
            m_thread = std::thread(&SensorManager::run, this);
        }

        void stop() {
            m_running = false;
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }
    };
}
