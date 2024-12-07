let allFlights = [];

        document.getElementById('addFlightButton').addEventListener('click', function() {
            document.getElementById('addFlightModal').style.display = 'block';
        });

        document.querySelector('.close').addEventListener('click', function() {
            document.getElementById('addFlightModal').style.display = 'none';
        });

        window.addEventListener('click', function(event) {
            if (event.target == document.getElementById('addFlightModal')) {
                document.getElementById('addFlightModal').style.display = 'none';
            }
        });

        document.getElementById('addFlightForm').addEventListener('submit', function(event) {
            event.preventDefault();
            const formData = new FormData(event.target);
            const data = {
                flight_number: formData.get('flightNumber'),
                max_passengers: parseInt(formData.get('maxPassengers'), 10),
                departure_location: formData.get('departureLocation'),
                departure_time: formData.get('departureTime'),
                arrival_location: formData.get('arrivalLocation'),
                arrival_time: formData.get('arrivalTime'),
                price: parseFloat(formData.get('price'))
            };
            fetch('/api/flights', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            })
            .then(response => response.json().then(data => ({ status: response.status, body: data })))
            .then(result => {
                if (result.status === 201) {
                    showMessage('航班增加成功', 'success');
                    fetchFlights();
                    document.getElementById('addFlightModal').style.display = 'none';
                } else {
                    showMessage('航班增加失败: ' + result.body.error, 'error');
                }
            })
            .catch(error => {
                showMessage('航班增加失败: ' + error.message, 'error');
            });
        });

        function fetchFlights() {
            fetch('/api/flights')
            .then(response => response.json().then(data => ({ status: response.status, body: data })))
            .then(result => {
                if (result.status === 200) {
                    allFlights = result.body;
                    updateFlightsTable(allFlights);
                    updateFilterOptions();
                } else {
                    showMessage('获取航班列表失败: ' + result.body.error, 'error');
                }
            })
            .catch(error => {
                showMessage('获取航班列表失败: ' + error.message, 'error');
            });
        }

        function updateFlightsTable(flights) {
            const flightsTableBody = document.getElementById('flightsTable').querySelector('tbody');
            flightsTableBody.innerHTML = '';
            flights.forEach(flight => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${flight.flight_number}</td>
                    <td>${flight.max_passengers}</td>
                    <td>${flight.departure_location}</td>
                    <td>${flight.departure_time}</td>
                    <td>${flight.arrival_location}</td>
                    <td>${flight.arrival_time}</td>
                    <td>${flight.price}</td>
                    <td>${flight.available_seats}</td>
                    <td>${flight.is_cancelled ? '已取消' : '正常'}</td>
                    <td class="countdown" data-departure="${flight.departure_time}" data-cancel-time="${flight.adjusted_cancel_time}"></td>
                    <td>
                        ${flight.is_cancelled ? 
                            `<button class="restore-button" onclick="restoreFlight('${flight.flight_number}')">恢复</button>` :
                            `<button class="cancel-button" onclick="cancelFlight('${flight.flight_number}')">取消</button>`
                        }
                        <button class="delete-button" onclick="deleteFlight('${flight.flight_number}')">删除</button>
                    </td>
                `;
                flightsTableBody.appendChild(row);
            });
            updateCountdowns();
        }

        function updateCountdowns() {
            const countdownElements = document.querySelectorAll('.countdown');
            countdownElements.forEach(element => {
                const departureTime = new Date(element.dataset.departure).getTime();
                const cancelTime = parseInt(element.dataset.cancelTime) * 1000; // Convert to milliseconds
                const now = new Date().getTime();
                const timeUntilDeparture = departureTime - now;
                const timeUntilCancel = timeUntilDeparture - cancelTime;

                if (timeUntilCancel > 0) {
                    const days = Math.floor(timeUntilCancel / (1000 * 60 * 60 * 24));
                    const hours = Math.floor((timeUntilCancel % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
                    const minutes = Math.floor((timeUntilCancel % (1000 * 60 * 60)) / (1000 * 60));
                    const seconds = Math.floor((timeUntilCancel % (1000 * 60)) / 1000);

                    element.textContent = `${days}天 ${hours}小时 ${minutes}分 ${seconds}秒`;
                } else {
                    element.textContent = '已过自动取消时间';
                }
            });
        }

        setInterval(updateCountdowns, 1000);

        function cancelFlight(flightNumber) {
            fetch(`/api/flights/cancel/${flightNumber}`, {
                method: 'PUT'
            })
            .then(response => response.json().then(data => ({ status: response.status, body: data })))
            .then(result => {
                if (result.status === 200) {
                    showMessage('航班取消成功', 'success');
                    fetchFlights();
                } else {
                    showMessage('航班取消失败: ' + result.body.error, 'error');
                }
            })
            .catch(error => {
                showMessage('航班取消失败: ' + error.message, 'error');
            });
        }

        function restoreFlight(flightNumber) {
            fetch(`/api/flights/restore/${flightNumber}`, {
                method: 'PUT'
            })
            .then(response => response.json().then(data => ({ status: response.status, body: data })))
            .then(result => {
                if (result.status === 200) {
                    showMessage('航班恢复成功', 'success');
                    fetchFlights();
                } else {
                    showMessage('航班恢复失败: ' + result.body.error, 'error');
                }
            })
            .catch(error => {
                showMessage('航班恢复失败: ' + error.message, 'error');
            });
        }

        function deleteFlight(flightNumber) {
            fetch(`/api/flights/${flightNumber}`, {
                method: 'DELETE'
            })
            .then(response => response.json().then(data => ({ status: response.status, body: data })))
            .then(result => {
                if (result.status === 200) {
                    showMessage('航班删除成功', 'success');
                    fetchFlights();
                } else {
                    showMessage('航班删除失败: ' + result.body.error, 'error');
                }
            })
            .catch(error => {
                showMessage('航班删除失败: ' + error.message, 'error');
            });
        }

        function showMessage(message, type) {
            const messageDiv = document.getElementById('message');
            messageDiv.textContent = message;
            messageDiv.className = 'message ' + type;
            messageDiv.style.display = 'block';
            setTimeout(() => {
                messageDiv.style.display = 'none';
            }, 3000);
        }

        function updateFilterOptions() {
            const departures = [...new Set(allFlights.map(f => f.departure_location))];
            const arrivals = [...new Set(allFlights.map(f => f.arrival_location))];

            const departureSelect = document.getElementById('filterDeparture');
            const arrivalSelect = document.getElementById('filterArrival');

            departureSelect.innerHTML = '<option value="">所有起飞地点</option>';
            departures.forEach(location => {
                departureSelect.innerHTML += `<option value="${location}">${location}</option>`;
            });

            arrivalSelect.innerHTML = '<option value="">所有降落地点</option>';
            arrivals.forEach(location => {
                arrivalSelect.innerHTML += `<option value="${location}">${location}</option>`;
            });
        }

        document.getElementById('filterFlightNumber').addEventListener('input', applyFilters);
        document.getElementById('filterDeparture').addEventListener('change', applyFilters);
        document.getElementById('filterArrival').addEventListener('change', applyFilters);
        document.getElementById('filterDate').addEventListener('change', applyFilters);
        document.getElementById('resetFilter').addEventListener('click', resetFilters);

        function applyFilters() {
            const flightNumber = document.getElementById('filterFlightNumber').value.toLowerCase();
            const departure = document.getElementById('filterDeparture').value;
            const arrival = document.getElementById('filterArrival').value;
            const date = document.getElementById('filterDate').value;

            let filteredFlights = allFlights;

            if (flightNumber) {
                filteredFlights = filteredFlights.filter(f => f.flight_number.toLowerCase().includes(flightNumber));
            }
            if (departure) {
                filteredFlights = filteredFlights.filter(f => f.departure_location === departure);
            }
            if (arrival) {
                filteredFlights = filteredFlights.filter(f => f.arrival_location === arrival);
            }
            if (date) {
                filteredFlights = filteredFlights.filter(f => f.departure_time.startsWith(date));
            }

            updateFlightsTable(filteredFlights);
        }

        function resetFilters() {
            document.getElementById('filterFlightNumber').value = '';
            document.getElementById('filterDeparture').value = '';
            document.getElementById('filterArrival').value = '';
            document.getElementById('filterDate').value = '';
            updateFlightsTable(allFlights);
        }

        document.getElementById('refreshButton').addEventListener('click', fetchFlights);

        document.getElementById('exportButton').addEventListener('click', function() {
            const csvContent = "data:text/csv;charset=utf-8," 
                + "航班号,最大载客数,起飞地点,起飞时间,降落地点,降落时间,单价,剩余座位数,取消状态\n"
                + allFlights.map(f => `${f.flight_number},${f.max_passengers},${f.departure_location},${f.departure_time},${f.arrival_location},${f.arrival_time},${f.price},${f.available_seats},${f.is_cancelled ? '已取消' : '正常'}`).join("\n");

            const encodedUri = encodeURI(csvContent);
            const link = document.createElement("a");
            link.setAttribute("href", encodedUri);
            link.setAttribute("download", "flights.csv");
            document.body.appendChild(link);
            link.click();
        });

        function updateCurrentTime() {
            const now = new Date();
            const formattedTime = now.getFullYear() + '-' +
                String(now.getMonth() + 1).padStart(2, '0') + '-' +
                String(now.getDate()).padStart(2, '0') + 'T' +
                String(now.getHours()).padStart(2, '0') + ':' +
                String(now.getMinutes()).padStart(2, '0') + ':' +
                String(now.getSeconds()).padStart(2, '0') + '.' +
                String(now.getMilliseconds()).padStart(3, '0');
            document.getElementById('currentTime').textContent = formattedTime;
        }

        setInterval(updateCurrentTime, 1000);
        updateCurrentTime();

        // 初始化时获取航班列表
        fetchFlights();